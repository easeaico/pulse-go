import torch
import neural_ode.models.utils as utils
import matplotlib
import matplotlib.pyplot as plt
import seaborn as sns

torch.set_default_tensor_type(torch.DoubleTensor)
# for headless use (write to files, don't show())
# matplotlib.use("Agg")

from typing import Literal, Optional, List, Tuple, Dict
import itertools
import collections
import ubelt as ub
import numpy as np
import pandas as pd
from dataclasses import asdict, astuple, dataclass
from contextlib import contextmanager

import pytorch_lightning as pl
import pytorch_forecasting as pf

# for cli to pick up
from neural_ode.models.recurrent import RecurrentODE
from neural_ode.models.seq2seq import Seq2Seq
from neural_ode.models.vae import VAE


def calc_cvcs(map_true, map_pred):
    THRESH = 40
    true_ixs = []
    pred_ixs = []
    for pt in range(len(map_true)):
        if len(ixs := torch.argwhere(map_true[pt] < THRESH)) > 0:
            true_ixs.append(ixs[0][0])
        else:
            true_ixs.append(-1)

        if len(ixs := torch.argwhere(map_pred[pt] < THRESH)) > 0:
            pred_ixs.append(ixs[0][0])
        else:
            pred_ixs.append(-1)

    true_ixs = np.array(true_ixs)
    pred_ixs = np.array(pred_ixs)

    import sklearn
    from sklearn.metrics import confusion_matrix, f1_score
    print(confusion_matrix(true_ixs > 0, pred_ixs > 0))
    print(f1_score(true_ixs > 0, pred_ixs > 0))

    valid_ixs = (true_ixs > 0) & (pred_ixs > 0)
    diffs_ixs = pred_ixs[valid_ixs] - true_ixs[valid_ixs]
    diffs_sec = diffs_ixs * 100 * 0.02  # * stride * base_freq
    print(len(map_true), len(valid_ixs), np.mean(diffs_sec), np.std(diffs_sec))


class UpdateKLCoef(pl.Callback):
    # for VAE, currently unused

    def __init__(self, kl_first, wait_until_kl=10):
        self.kl_first = kl_first
        self.wait_until_kl = wait_until_kl
        self.kl_coef = 0.

    def on_train_epoch_end(self, trainer: "pl.Trainer",
                           pl_module: "pl.LightningModule") -> None:
        epoch = pl_module.current_epoch
        if epoch < self.wait_until_kl:
            self.kl_coef = 0.
        else:
            self.kl_coef = self.kl_first * (1 -
                                            0.99**(epoch - self.wait_until_kl))


# https://pytorch-lightning.readthedocs.io/en/stable/common/debugging.html


# dropping out of lightning like this shouldn't be necessary, but some things
# are not possible with pf and pl.Trainer:
    # 1. logging_metrics broken, must compute by hand
    #   https://github.com/jdb78/pytorch-forecasting/issues/1151
    # 2. aligning y_true and y_pred instead of just getting y_pred


# https://discuss.pytorch.org/t/opinion-eval-should-be-a-context-manager/18998/3
@contextmanager
def evaluating(net):
    '''Temporarily switch to evaluation mode.'''
    istrain = net.training
    try:
        net.eval()
        yield net
    finally:
        if istrain:
            net.train()

@dataclass
class Batch:  # TODO make x optional?
    y_pred: List[torch.Tensor]
    y_true: List[torch.Tensor]
    xs: List[torch.Tensor]
    yt: Optional[List[torch.Tensor]] = None
    xt: Optional[List[torch.Tensor]] = None

    def __post_init__(self):
        # stub out t if needed
        assert (self.yt is None) == (self.xt is None)
        if self.yt is None and 0:
            print('TODO this is probably broken')
            # self.use_t_mins = False
            self.xt = [torch.arange(0, len(x)) for x in self.xs[0]]
            self.yt = [
                torch.arange(0 + len(x),
                             len(y) + len(x))
                for x, y in zip(self.xs[0], self.y_pred[0])
            ]
        else:
            # self.use_t_mins = True
            pass

    @classmethod
    def from_net_out(cls, model, batch):
        with evaluating(model), torch.no_grad():
            # batch = trainer.datamodule.transfer_batch_to_device(
            # batch, trainer.model.device, 0)
            # or pf.utils.move_to_device
            x, y = batch
            # use model.to_prediction to support quantiles
            y_pred, y_true, xs = (model(x)['prediction'], y[0],
                                  x['encoder_target'])

        def _detach_fn(tensor, lens=None):
            tensor = tensor.detach().cpu()
            if lens is not None:
                tensor = [t[:l] for t, l in zip(tensor, lens)]
            return tensor

        xt, yt = (x['encoder_cont'][:, :, model.t_idx],
                  x['decoder_cont'][:, :, model.t_idx])

        return cls(
            [
                _detach_fn(y.squeeze(dim=-1), x['decoder_lengths'])
                for y in y_pred
            ],
            [_detach_fn(y, x['decoder_lengths']) for y in y_true],
            [_detach_fn(_x, x['encoder_lengths']) for _x in xs],
            [_detach_fn(yt, x['decoder_lengths'])],  # [] needed for agg
            [_detach_fn(xt, x['encoder_lengths'])],  # [] needed for agg
        )

    @classmethod
    def from_agg(cls, batches):

        assert all(b.xt is not None for b in batches), 'TODO'

        def _flatten(tups: List[Tuple]):
            '''
            [n_batches] [f] [b] t 1 -> [f] [n_batches * b] t 1
            with [] == list, 1 being the dim of each individual feature
            this is a huge pain with ragged arrays, and unreadable...
            '''
            lsts = zip(*tups)
            return [[[item for items in batch for item in items]
                     for batch in zip(*lst)] for lst in lsts]

        args = _flatten(map(astuple, batches))

        return cls(*args)


class PlotCallback(pl.Callback):

    def __init__(self, every_n_batches=1, every_n_epochs=10):
        self.every_n_batches = every_n_batches
        self.every_n_epochs = every_n_epochs
        self.ready = True
        self.tr_data: List[Batch] = []
        self.va_data: List[Batch] = []

    def on_sanity_check_start(self, trainer, pl_module) -> None:
        self.ready = False

    def on_sanity_check_end(self, trainer, pl_module) -> None:
        self.ready = True

    def on_train_batch_end(self, trainer, pl_module, outputs, batch,
                           batch_idx):
        if (self.ready and (batch_idx % self.every_n_batches == 0)
                and (trainer.current_epoch % self.every_n_epochs == 0)):
            self.tr_data.append(Batch.from_net_out(trainer.model, batch))

    def on_validation_batch_end(self, trainer, pl_module, outputs, batch,
                                batch_idx, dataloader_idx):
        if (self.ready and (batch_idx % self.every_n_batches == 0)
                and (trainer.current_epoch % self.every_n_epochs == 0)):
            self.va_data.append(Batch.from_net_out(trainer.model, batch))

    @staticmethod
    def make_plot(
        y_pred: List[torch.Tensor],
        y_true: List[torch.Tensor],
        yt: List[torch.Tensor],
        xt: List[torch.Tensor],
        xs: Optional[torch.Tensor] = None,
    ) -> Tuple[np.ndarray, Dict[str, float]]:

        target_labels_to_facets = {
            'HeartRate(1/min)': 0,
            'BloodVolume(mL)': 1,
            'CardiacOutput(mL/min)': 2,
            'RespirationRate(1/min)': 0,
            'OxygenSaturation': 3,
            'MeanArterialPressure(mmHg)': 4,
            'SystolicArterialPressure(mmHg)': 4,
            'DiastolicArterialPressure(mmHg)': 4,
            'HemoglobinContent(g)': 5,
            'TotalHemorrhageRate(mL/s)': 3,
            'TotalHemorrhagedVolume(mL)': 1,
        }
        # add target names to title
        facets_to_target_labels = {
            v: [kk for kk, vv in target_labels_to_facets.items() if vv == v]
            for v in set(target_labels_to_facets.values())
        }
        target_labels_to_facets = {
            k: '\n'.join([str(v)] + facets_to_target_labels[v])
            for k, v in target_labels_to_facets.items()
        }

        # (target pt t 1)
        # (11 3 27 1)
        if xs is None:
            xs = [[torch.zeros((0, 1)) for y in y_pred[0]]] * len(y_pred)

        # (b * n_batches) t -> t
        xt = np.concatenate(xt[0]) / 60
        yt = np.concatenate(yt[0]) / 60

        # TODO use group_id for this after adding data aug
        pts = np.concatenate(
            [np.repeat(i, len(y)) for i, y in enumerate(y_pred[0])])

        pred_df = pd.DataFrame(
            dict(zip(target_labels_to_facets.keys(),
                     [np.concatenate(y).squeeze() for y in y_pred]),
                 t=yt,
                 pt=pts))
        true_df = pd.DataFrame(
            dict(
                zip(target_labels_to_facets.keys(),
                    [np.concatenate(y).squeeze() for y in y_true])))

        df = pred_df.melt(id_vars=['t', 'pt'],
                          var_name='vital',
                          value_name='pred').join(
                              true_df.melt(value_name='true')['true'])

        ROLL = 0
        if ROLL:
            df[['pred', 'true']] = (df.groupby('pt')[['pred', 'true']]
                                      .transform(
                                          lambda s: s.rolling(window=100).mean()))
        df = df.dropna()

        # gets rid of start of THR/THV as well as padded timesteps
        df_error = df[df['true'] != 0]
        # https://stackoverflow.com/a/53954986
        with pd.option_context('mode.chained_assignment', None):
            df_error['rel_err'] = (df_error['pred'] -
                                   df_error['true']) / df_error['true']

        # add x timesteps and reshape long
        df['is_y'] = '1_y_predeath'
        if len(xt) > 0:
            pts_x = np.concatenate(
                [np.repeat(i, len(x)) for i, x in enumerate(xs[0])])
            df_x = pd.DataFrame(
                dict(zip(target_labels_to_facets.keys(),
                         [np.concatenate(x).squeeze() for x in xs]),
                     t=xt,
                     pt=pts_x)).melt(id_vars=['t', 'pt'], var_name='vital')
            df_x['pred'] = df_x['value']
            df_x['true'] = df_x['value']
            df_x.drop('value', axis=1, inplace=True)
            df_x['is_y'] = '0_x'
            df = pd.concat((df, df_x), ignore_index=True, copy=False)
        df = df.melt(id_vars=['t', 'pt', 'vital', 'is_y'])
        df['facet'] = df['vital'].replace(target_labels_to_facets)

        # units= for groupid (patient splits by death time etc) w/ estimator

        def to_array(grid):
            grid.fig.canvas.draw()
            buf = grid.fig.canvas.buffer_rgba()
            arr = np.array(buf)[:, :, :3]
            grid.fig.clf()
            plt.close()
            return arr

        # pred and true timeseries
        grid1 = sns.relplot(
            kind='line',
            estimator=None,
            x='t',
            y='value',
            row='variable',
            col='facet',
            hue='vital',
            data=df,
            alpha=0.5,
            sort=False,
            legend=False,
            facet_kws=dict(sharey='col'),
            units='pt',
            style='is_y',
            markers=False,  # ['o', '.', '.'],
            style_order=['0_x', '1_y_predeath', '1_y_postdeath'],
            linewidth=1,
            # ms=1,
        )
        # add green and yellow points
        for ax, targets in zip(grid1.axes[1, :],
                               facets_to_target_labels.values()):
            batch_size = len(ax.lines) // (2 * len(targets))
            for line in np.take(ax.lines, [
                    i
                    for i in range(len(ax.lines)) if (i // batch_size) % 2 == 0
            ]):
                ax.plot(line.get_xdata()[0], line.get_ydata()[0], 'go', ms=2)
                ax.plot(line.get_xdata()[-1], line.get_ydata()[-1], 'yo', ms=2)
        for ax, targets in zip(grid1.axes[0, :],
                               facets_to_target_labels.values()):
            batch_size = len(ax.lines) // (2 * len(targets))
            for line in np.take(ax.lines, [
                    i
                    for i in range(len(ax.lines)) if (i // batch_size) % 2 == 0
            ]):
                ax.plot(line.get_xdata()[0], line.get_ydata()[0], 'go', ms=2)
                ax.plot(line.get_xdata()[-1], line.get_ydata()[-1], 'yo', ms=2)
        grid1.set_xlabels('t (min)')
        width, height = grid1.figure.get_size_inches()
        arr1 = to_array(grid1)

        # relative error
        grid2 = sns.relplot(kind='line',
                            estimator=None,
                            x='t',
                            y='rel_err',
                            row=None,
                            col='vital',
                            hue='vital',
                            data=df_error,
                            alpha=0.4,
                            facet_kws=dict(sharex=False, sharey=False),
                            sort=False,
                            units='pt')
        # add info to subtitles
        errs = []
        for ax in grid2.axes.flat:
            title = ax.get_title()
            err = np.concatenate([line.get_ydata()
                                  for line in ax.lines]).mean()
            errs.append(err)
            ax.set_title(f'{title}\nmean relative error = {err:.2f}')
        grid2.set_xlabels('t (min)')
        # resize for stackability
        cur_w, cur_h = grid2.figure.get_size_inches()
        grid2.figure.set_size_inches(width, (width * cur_h / cur_w) * 1.2)
        arr2 = to_array(grid2)

        return (np.vstack(
            (arr1, arr2)), dict(zip(target_labels_to_facets.keys(), errs)))

    # @rank_zero_only
    def on_train_epoch_end(self, trainer: pl.Trainer,
                           pl_module: pl.LightningModule) -> None:
        if self.ready and self.tr_data:
            # predictions = trainer.predict(trainer.model, dataloaders=trainer.datamodule.train_dataloader())

            arr, errs = self.make_plot(**asdict(Batch.from_agg(self.tr_data)))

            trainer.logger.experiment.add_image(
                'train',
                arr,
                global_step=trainer.global_step,
                dataformats='HWC')
            for k, v in errs.items():
                escaped_key = k  # .replace('/', '//')
                trainer.logger.experiment.add_scalar(
                    f'mpe/train/{escaped_key}', v, trainer.global_step)

            self.tr_data = []

    # @rank_zero_only
    def on_validation_epoch_end(self, trainer: pl.Trainer,
                                pl_module: pl.LightningModule) -> None:
        if self.ready and self.va_data:
            arr, errs = self.make_plot(**asdict(Batch.from_agg(self.va_data)))

            trainer.logger.experiment.add_image(
                'val', arr, global_step=trainer.global_step, dataformats='HWC')
            for k, v in errs.items():
                escaped_key = k  # .replace('/', '//')
                trainer.logger.experiment.add_scalar(f'mpe/val/{escaped_key}',
                                                     v, trainer.global_step)

            self.va_data = []


from pytorch_lightning.utilities.cli import LightningCLI
'''
could try this if problems with loading ckpts
https://github.com/Lightning-AI/lightning/issues/12302#issuecomment-1110425635
class NBCLI(LightningCLI):
    def fit(self, *args, **kwargs):
        pass
'''


class MyLightningCLI(LightningCLI):
    '''
    Currently identical to LightningCLI. Reserved for .from_dataset() shenanigans.
    '''

    # def _get(self, config: Dict[str, Any], key: str, default: Optional[Any] = None) -> Any:
    # """Utility to get a config value which might be inside a subcommand."""
    # return config.get(str(self.subcommand), config).get(key, default)

    # subclass this fn to couple model to dm and delay its instantiation until
    # after dm is setup.
    def instantiate_classes(self) -> None:
        """Instantiates the classes and sets their attributes."""
        self.config_init = self.parser.instantiate_classes(self.config)
        self.datamodule = self._get(self.config_init, "data")
        self.model = self._get(self.config_init, "model")

        # self.model = self.model.from_datamodule(self.datamodule, **self.model.hparams)

        self._add_configure_optimizers_method_to_model(self.subcommand)
        self.trainer = self.instantiate_trainer()

    def before_fit(self):
        pass

    def before_validate(self):
        pass

    def before_test(self):
        pass

    def before_predict(self):
        pass

    def before_tune(self):
        pass


# TODO curriculum learning https://arxiv.org/abs/2101.10382
# train on easy examples before hard examples
# with missingness, this amounted to removing missingness from 10 more points
# per batch every epoch
# this was done only for enc-dec arch, related to GoogleStock(start_reverse)?
# https://gitlab.kitware.com/physiology/engine/-/blob/study/ode/src/python/pulse/study/neural_ode/neural_ode/run_models.py#L263
from jsonargparse import lazy_instance


def trainer_kwargs(load_ckpt=None):
    kl_getter = UpdateKLCoef(1)
    top_va_callback = pl.callbacks.ModelCheckpoint(dirpath='checkpoints',
                                                   save_top_k=1,
                                                   monitor='val_loss')
    top_tr_callback = pl.callbacks.ModelCheckpoint(dirpath='checkpoints',
                                                   save_top_k=1,
                                                   monitor='train_loss_epoch')
    plot_callback = PlotCallback(every_n_epochs=20)
    early_stopping_callback = pl.callbacks.EarlyStopping(
        monitor='train_loss_epoch',
        # monitor='tr_mse',
        # monitor='va_mse',
        mode='min',
        patience=30)
    lr_monitor_callback = pl.callbacks.LearningRateMonitor()
    logger = lazy_instance(
        pl.loggers.TensorBoardLogger,
        save_dir='lightning_logs/',
        name=None,  # can set this in cli
        default_hp_metric=True,
        log_graph=False,
    )
    return dict(
        max_epochs=301,
        check_val_every_n_epoch=1,
        resume_from_checkpoint=load_ckpt,
        callbacks=[
            kl_getter,
            plot_callback,
            early_stopping_callback,
            lr_monitor_callback,
            # top_va_callback,
            # top_tr_callback,
        ],
        # gradient_clip_val=0.01,
        gradient_clip_val=1.,
        # gradient_clip_val=42.14720820598181,  # TFT optimize_hyperparams
        logger=logger,
        gpus=1)


class IdMetric(pf.MultiHorizonMetric):
    """
    Dummy metric to validate reduction/masking calcs. Always returns 1.
    """

    def loss(self, y_pred, target):
        loss = torch.ones_like(target)
        return loss


def scan_metric(
        model,
        dm,
        xt_starts_mins=[0, 20, 40],
        xt_ends_mins=np.arange(10, 100, 10),
        # xt_ends_mins=[20],
        metric=pf.MAPE(),
        # metric=IdMetric(reduction='sum'),
        train=True):

    # ensure metric
    # TODO skip MAE/MAPE step when y_true == 0
    # regular `in` check doesn't work here, don't know why
    # if not any(metric.name == m.name for m in model.logging_metrics):
        # model.logging_metrics.append(metric)
    # model.logging_metrics.append(pf.MAE())

    # run model and aggregate data
    dct = collections.defaultdict(list)
    kws = {
        'train': dict(dset_nm='df_tr', xt_restrict=True),
        'val': dict(dset_nm='df_va', xt_restrict=False),
        # 'test': dict(dset_nm='df_te', xt_restrict=False),
    }
    for xt_start_mins, xt_end_mins in filter(
            lambda tup: tup[0] < tup[1],
            itertools.product(xt_starts_mins, xt_ends_mins)):

        for nm, kwargs in kws.items():
            dl = dm._dataloader(**kwargs, xt_mins=(xt_start_mins, xt_end_mins))

            # print(nm, xt_start_mins, xt_end_mins)
            batch = Batch.from_agg([Batch.from_net_out(model, b) for b in dl])
            # print('...evaled')
            metric_dct = {}
            for i, trg in enumerate(model.target):
                tt, pp = batch.y_true[i], batch.y_pred[i]
                true = torch.nn.utils.rnn.pack_sequence(batch.y_true[i],
                                                        enforce_sorted=False)
                pred = torch.nn.utils.rnn.pack_sequence(batch.y_pred[i],
                                                        enforce_sorted=False)
                # TODO make real metrics work
                # pred = torch.nn.utils.rnn.pad_sequence(pred, batch_first=True)
                # metric_dct[trg] = type(metric)()(true, pred)
                metric_dct[trg] = torch.mean(
                    torch.cat([
                        (t-p).abs() / t.abs() for t,p in zip(tt, pp)])).item()

            n_timesteps = 0
            for x, _ in dl:
                n_timesteps += x['decoder_lengths'].sum()
            n_timesteps = n_timesteps.item()
            n_mins = n_timesteps * utils.BASE_FREQ_S * dm.stride / 60

            metric_dct.update(xt_start_mins=xt_start_mins,
                              xt_end_mins=xt_end_mins,
                              n_pts=len(dl.dataset),
                              n_timesteps=n_timesteps,
                              n_mins=n_mins,
                              dset=nm,
            )
            for k, v in metric_dct.items():
                dct[k].append(v)

    # plot
    var = metric.name
    ix_vars = ['xt_start_mins', 'xt_end_mins', 'dset']
    data = pd.DataFrame.from_dict(dct)
    data['mins_per_pt'] = data['n_mins'] / data['n_pts']
    data = data.drop('n_mins', axis=1)
    data = data.melt(
        id_vars=['n_pts', 'mins_per_pt', 'n_timesteps'] + ix_vars, var_name=var)
    data['value'] = data['value'].replace([np.inf, -np.inf], np.nan)
    data = data.dropna(axis=0, subset='value')

    grid = sns.relplot(
        kind='line',
        estimator=None,
        data=data,
        x='xt_end_mins',
        y='value',
        col='xt_start_mins',
        row='dset',
        hue=var,
        alpha=1,
        sort=False,
        legend=True,
        facet_kws=dict(),
        # units='pt',  # should use estimator if this is available
        markers=True,  # ['o', '.', '.'],
        linewidth=2,
        # ms=1,
    )


    # https://stackoverflow.com/a/65799913
    def twin_lineplot(x, **kwargs):
        ax = plt.twinx()
        ax.set_ylim(0, 200)  # TODO figure out how to eq this
        sub_df = (data.loc[x.index, ix_vars +
                           ['n_pts', 'mins_per_pt']].drop_duplicates(ix_vars).melt(
                               id_vars=ix_vars, var_name='variable'))
        sns.lineplot(data=sub_df,
                     x='xt_end_mins',
                     y='value',
                     style='variable',
                     ax=ax,
                     **kwargs)

    grid.map(
        twin_lineplot,
        'xt_end_mins',
        color='k',
        markers=True,
        legend=True,
    )
    plt.show()


def cvc(model, dl):

    batch = Batch.from_agg([Batch.from_net_out(model, batch) for batch in dl])
    map_ix = model.target.index('MeanArterialPressure(mmHg)')  # 5

    true = torch.nn.utils.rnn.pad_sequence(batch.y_true[map_ix],
                                           batch_first=True)
    pred = torch.nn.utils.rnn.pad_sequence(batch.y_pred[map_ix],
                                           batch_first=True)
    prev = torch.nn.utils.rnn.pad_sequence(batch.xs[map_ix], batch_first=True)

    map_cutoff = 40

    # need to negate to sort by descending order
    true_ixs = torch.searchsorted(-true,
                                  torch.tensor([[-map_cutoff]] * len(true)))
    pred_ixs = torch.searchsorted(-pred,
                                  torch.tensor([[-map_cutoff]] * len(pred)))
    prev_ixs = torch.searchsorted(-prev,
                                  torch.tensor([[-map_cutoff]] * len(prev)))

    t = torch.nn.utils.rnn.pad_sequence(batch.yt[0],
                                        batch_first=True,
                                        padding_value=-1)
    xt = torch.nn.utils.rnn.pad_sequence(batch.xt[0],
                                         batch_first=True,
                                         padding_value=-1)
    lens = torch.searchsorted(-t, torch.tensor([[1]] * len(t)))
    xlens = torch.searchsorted(-xt, torch.tensor([[1]] * len(xt)))

    has_true, has_pred, has_prev = (true_ixs < lens, pred_ixs < lens,
                                    prev_ixs < xlens)

    # exclude pts in cvc during x
    has_prev = has_prev.squeeze()
    true_ixs, pred_ixs = true_ixs[~has_prev], pred_ixs[~has_prev]
    t = t[~has_prev]
    has_true, has_pred = has_true[~has_prev], has_pred[~has_prev],

    dt = (t.take_along_dim(pred_ixs.where(has_pred, torch.tensor(0)), 1) -
          t.take_along_dim(true_ixs.where(has_true, torch.tensor(0)),
                           1))[has_true & has_pred]

    from sklearn.metrics import confusion_matrix, f1_score, balanced_accuracy_score
    print(confusion_matrix(has_true, has_pred))
    f1 = f1_score(has_true, has_pred)
    ba = balanced_accuracy_score(has_true, has_pred)
    print(f'{f1=} {ba=}')
    print(torch.mean(dt), torch.std(dt))

    # plt.hist(dt.numpy(), bins=30)
    # plt.show()


if __name__ == "__main__":

    torch.set_default_dtype(torch.float32)

    # for mild dset
    # --data.root_path=/data/pulse/hemorrhage/patient_variability/hemorrhage/test

    # model = pf.DeepAR.from_dataset(dm.dset_tr, learning_rate=1e-4)

    cli = MyLightningCLI(model_class=utils.PFMixin,
                         datamodule_class=utils.BaseData,
                         trainer_defaults=trainer_kwargs(),
                         seed_everything_default=47,
                         run=False,
                         subclass_mode_model=True,
                         subclass_mode_data=True,
                         auto_registry=False)

    print('reloading model')
    if ckpt_path := cli.trainer.resume_from_checkpoint:
        # TODO ensure old vs new dm settings match here
        # https://github.com/Lightning-AI/lightning/issues/924
        cli.model = cli.model.from_datamodule(cli.datamodule,
                                              ckpt_path=ckpt_path)
    else:
        cli.model = cli.model.from_datamodule(cli.datamodule,
                                              **cli.model.hparams)
    # would need this if optimizer or lr scheduler is set through cli
    # cli._add_configure_optimizers_method_to_model(None)

    if cli.trainer.overfit_batches:
        cli.datamodule.shuffle = False  # HACK
        # TODO change lr scheduler from val_loss to train_loss_epoch here

    # pretty log name for tensorboard
    # to set manually:
    # --trainer.logger TensorBoardLogger
    # --trainer.logger.save_dir lightning_logs
    # --trainer.logger.name xxxx
    # https://jsonargparse.readthedocs.io/en/stable/index.html#default-values
    if (cli.trainer.logger
            and (cli.config.trainer.logger.init_args.name is None)):
        dset_names = {
            '/data/pulse/hemorrhage/hemorrhage': 'orig',
            '/data/pulse/hemorrhage/patient_variability/hemorrhage/test':
            'mild'
        }
        b = cli.datamodule.batch_size
        s = cli.datamodule.stride
        if cli.trainer.overfit_batches:
            p = cli.trainer.overfit_batches * b
        else:
            p = len(cli.datamodule._dset('df_tr', **cli.datamodule.kwargs))
        st = cli.datamodule.static_behavior[0]

        new_name = '_'.join((
            type(cli.model).__name__,
            dset_names[str(cli.datamodule.root_path)],
            f'b{b}'  # batches
            f'p{p}'  # patients
            f's{s}'  # stride
            f'st{st}'  # static behavior ignore, propagate, augment
        ))
        kwargs = dict(cli.config.trainer.logger.init_args)
        kwargs.update(name=new_name)
        cli.trainer.logger = type(cli.trainer.logger)(**kwargs)

    # batch = next(iter(cli.datamodule.train_dataloader()))
    # x, y = batch
    # y_pred = cli.model(x)

    # DUMP HPARAMS
    # from pytorch_lightning.core.saving import save_hparams_to_yaml
    # save_hparams_to_yaml('cli_model.yaml', cli.model.hparams)
    # save_hparams_to_yaml('cli_dm.yaml', cli.datamodule.hparams)

    # # fit == train + validate
    # cli.trainer.fit(cli.model, datamodule=cli.datamodule)
    # scan_metric(cli.model, cli.datamodule)
    # cvc(cli.model, cli.datamodule._dataloader('df_tr', train=False))
    # cvc(cli.model, cli.datamodule.val_dataloader())
    # cvc(cli.model, cli.datamodule.test_dataloader())

    # dl = cli.datamodule._dataloader('df_tr', train=False)
    # dl = cli.datamodule.test_dataloader()
    import itertools
    dl = itertools.chain(
        cli.datamodule._dataloader('df_tr', train=False),
        cli.datamodule._dataloader('df_va', train=False),
        cli.datamodule._dataloader('df_te', train=False)
    )
    batch = Batch.from_agg([Batch.from_net_out(cli.model, batch) for batch in dl])
    breakpoint()
    PlotCallback.make_plot(**asdict(batch))
    # yt, yp = batch.y_true[5], batch.y_pred[5]
    # calc_cvcs(yt, yp)
    cvc(cli.model, dl)

