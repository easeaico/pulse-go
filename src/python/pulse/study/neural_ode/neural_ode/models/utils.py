###########################
# Modified from: https://github.com/YuliaRubanova/latent_ode by Kirin Ciao
###########################
import os
import sys
import csv
import time
import logging
import pickle
import math
import warnings
import torch
import torch.nn as nn
import torchcde
import numpy as np
import pandas as pd
import json
import einops
import ubelt as ub
from copy import deepcopy
from dataclasses import dataclass, field, asdict
from typing import Optional, List, Literal, Any, Dict, Tuple
import pickle as pkl
import functools

import pytorch_lightning as pl
import pytorch_forecasting as pf
from pytorch_forecasting.data.timeseries import (TimeSeriesDataSet,
                                                 GroupNormalizer)
import torchmetrics

from neural_ode.models.evaluation import get_log_likelihood, get_mse

# TODO death prediction
# https://pytorch-forecasting.readthedocs.io/en/stable/tutorials/building.html#Classification


@dataclass
class BaseModelWithCovariatesKwargs:
    static_categoricals: List[str] = field(default_factory=list)
    static_reals: List[str] = field(default_factory=list)
    time_varying_categoricals_encoder: List[str] = field(default_factory=list)
    time_varying_categoricals_decoder: List[str] = field(default_factory=list)
    categorical_groups: Dict[str, List[str]] = field(default_factory=dict)
    time_varying_reals_encoder: List[str] = field(default_factory=list)
    time_varying_reals_decoder: List[str] = field(default_factory=list)
    embedding_sizes: Dict[str, Tuple[int, int]] = field(default_factory=dict)
    embedding_paddings: List[str] = field(default_factory=list)
    embedding_labels: Dict[str, np.ndarray] = field(default_factory=dict)
    x_reals: List[str] = field(default_factory=list)
    x_categoricals: List[str] = field(default_factory=list)
    hidden_size: Optional[int] = None


class PFMixin(pl.LightningModule):

    @staticmethod
    def _coerce_loss(loss, n_targets=1, weights=None):
        if isinstance(loss, str):
            _loss = getattr(pf.metrics, loss)()
        elif isinstance(loss, (pf.metrics.Metric, pf.metrics.MultiLoss)):
            _loss = loss
        else:
            raise TypeError(loss)

        if n_targets > 1:
            if not isinstance(_loss, pf.metrics.MultiLoss):
                _loss = pf.metrics.MultiLoss([_loss] * n_targets, weights)

        return _loss

    @staticmethod
    def target_weights(dset: pf.TimeSeriesDataSet) -> List[float]:
        '''
        calculate target weights
        (pass this to MultiLoss in model)
        '''
        # df.melt(id_vars=['X', 'Y']).query('value == 1')
        # TODO use this for normalizer as well
        first_entries = torch.stack([
            dset.data['target'][i][dset.index['index_start'].values]
            for i in range(len(dset.target))
        ])
        initial_target_values = first_entries.mean(axis=1)
        mean_target_values = torch.tensor(
            [dset.data['target'][i].mean() for i in range(len(dset.target))])
        # targets 'TotalHemorrhageRate(mL/s)', 'TotalHemorrhagedVolume(mL)'
        # start at 0, so use their mean values instead of initial values
        return 1. / torch.where(initial_target_values > 0,
                                initial_target_values, mean_target_values)

    def __init__(
            self,
            learning_rate=1e-2,
            # loss=pf.metrics.RMSE(),
            loss='MAE',  # coerce later to avoid yaml bug in LightningCLI(run=True)
            # loss=pf.metrics.SMAPE(),
            # logging_metrics=[pf.metrics.RMSE()],
        logging_metrics=['MAE'],
            optimizer='Adamax',  # 'ranger'
            reduce_on_plateau_patience=16,
            reduce_on_plateau_reduction=2,
            reduce_on_plateau_min_lr=1e-6,
            weight_decay=0.,
            optimizer_params={},
            monotone_constaints={},  # sp
            x_dims=None,
            y_dims=None,
            STUB=True,
            **kwargs):
        '''
        MAE is equal to:
        (torch.stack([(y_pred['prediction'][i].squeeze() - y[0][i]).abs().mean() for i in range(11)])).sum(0)
        model.loss(y_pred['prediction'], y)

        # specifically, it DOES respect decoder_lengths through PackedSequence
        result = []
        for i in range(11):
            b, t, f = prediction[i].shape
            assert f == 1
            subresult = []
           for j in range(b):
                yy = rnn.unpack_sequence(y[0][i])[j]
                subresult.append(
                    (prediction[i][j][:len(yy)].squeeze() - yy).abs()
                )
           result.append(torch.concat(subresult).mean())

        assert torch.allclose(
            (torch.stack(result).cpu() * self.loss.weights).sum(),
            self.loss(prediction, y).cpu()
        )
        '''
        self.save_hyperparameters(ignore=['x_dims', 'y_dims', 'STUB'])
        # self.save_hyperparameters(dict(loss=loss, logging_metrics=logging_metrics), logger=False)
        # TODO ensure the cov kwargs are saved in hparams
        self.STUB = STUB
        if not STUB:
            super().__init__(loss=loss,
                             logging_metrics=logging_metrics,
                             **kwargs)

    # TODO check this when implementing categorical target in from_dataset
    # https://pytorch-forecasting.readthedocs.io/en/stable/tutorials/building.html#Predicting-multiple-targets-at-the-same-time
    @classmethod
    def from_datamodule(cls, dm, ckpt_path=None, **kwargs):
        dm.prepare_data()
        dm.setup(stage='train')
        # TODO keep track of deduce_default_output_parameters output_size
        # output_size (Union[int, List[int]], optional): number of outputs
        # (e.g. number of quantiles for QuantileLoss and one target or list of
        # output sizes).
        # for TFT:
        # output_size=[7, 7, 7, 7], # 4 target variables
        dataset = dm._dset('df_tr', **dm.kwargs)

        # first-time initialization
        if ckpt_path is None:
            loss_weights = cls.target_weights(dataset)
            x_dims = len(dataset.reals) + len(dataset.flat_categoricals)
            y_dims = len(dataset.target)
            loss = cls._coerce_loss(kwargs['loss'], y_dims, loss_weights)
            # These are PER-TARGET, no need to dup+weight them
            logging_metrics = nn.ModuleList(
                [cls._coerce_loss(l) for l in kwargs['logging_metrics']])
            kwargs.update(
                x_dims=x_dims,
                y_dims=y_dims,
                loss=loss,
                logging_metrics=logging_metrics,
            )
            self = cls.from_dataset(
                dataset,
                **ub.dict_diff(
                    kwargs,
                    list(asdict(BaseModelWithCovariatesKwargs())) +
                    ['output_transformer', 'target', 'output_size']),
                STUB=False,
            )  #, output_transformer=dataset.target_normalizer)

            # HACK ODE
            # ckpt_path = 'lightning_logs/RecurrentODE_orig_b8p217s100sta/version_9/checkpoints/epoch=112-step=3051.ckpt'
            # self.load_state_dict(torch.load(ckpt_path)['state_dict'])
        else:
            self = cls.load_from_checkpoint(ckpt_path)

        # postprocessing

        batch = next(iter(dm.train_dataloader()))
        x, y = batch
        # https://pytorch-lightning.readthedocs.io/en/stable/api/pytorch_lightning.core.LightningModule.html#pytorch_lightning.core.LightningModule.example_input_array
        self.example_input_array = (x, )
        # self.batch_size = len(x['encoder_lengths'])
        self.batch_size = dm.batch_size
        self.t_idx = dataset.reals.index(dm.t_param)
        self.target = dataset.target
        # self.x_dims = x_dims  # TODO make properties based on pf?
        # self.y_dims = y_dims
        return self


class BaseModel(PFMixin, pf.BaseModelWithCovariates):
    '''

    Model with additional methods using covariates.

    Assumes the following hyperparameters:

    Args:
        static_categoricals (List[str]): names of static categorical variables
        static_reals (List[str]): names of static continuous variables
        time_varying_categoricals_encoder (List[str]): names of categorical variables for encoder
        time_varying_categoricals_decoder (List[str]): names of categorical variables for decoder
        time_varying_reals_encoder (List[str]): names of continuous variables for encoder
        time_varying_reals_decoder (List[str]): names of continuous variables for decoder
        x_reals (List[str]): order of continuous variables in tensor passed to forward function
        x_categoricals (List[str]): order of categorical variables in tensor passed to forward function
        embedding_sizes (Dict[str, Tuple[int, int]]): dictionary mapping categorical variables to tuple of integers
            where the first integer denotes the number of categorical classes and the second the embedding size
        embedding_labels (Dict[str, List[str]]): dictionary mapping (string) indices to list of categorical labels
        embedding_paddings (List[str]): names of categorical variables for which label 0 is always mapped to an
             embedding vector filled with zeros
        categorical_groups (Dict[str, List[str]]): dictionary of categorical variables that are grouped together and
            can also take multiple values simultaneously (e.g. holiday during octoberfest). They should be implemented
            as bag of embeddings
    '''
    '''
    problems with using setup() for partial initialization:
        https://github.com/Lightning-AI/lightning/issues/9943
        https://github.com/Lightning-AI/lightning/issues/5410
        https://github.com/Lightning-AI/lightning/issues/5586
        https://forums.pytorchlightning.ai/t/loading-checkpoints-when-models-built-using-a-setup-block/530/2
    '''

    def __init__(self, STUB=True, **kwargs):
        self.gaussian_likelihood_std = torch.tensor([0.01])
        if not STUB:
            kwargs = ub.dict_diff(
                kwargs, list(asdict(BaseModelWithCovariatesKwargs(
                ))))  # + ['output_transformer', 'target', 'output_size'])
        super().__init__(STUB=STUB, **kwargs)

        if not STUB:
            # create embedder - can be fed with x["encoder_cat"] or
            # x["decoder_cat"] and will return dictionary of category names
            # mapped to embeddings
            self.input_embeddings = pf.MultiEmbedding(
                embedding_sizes=self.hparams.embedding_sizes,
                categorical_groups=self.hparams.categorical_groups,
                embedding_paddings=self.hparams.embedding_paddings,
                x_categoricals=self.hparams.x_categoricals,
                max_embedding_size=self.hparams.hidden_size,
            )

        # https://pytorch-lightning.readthedocs.io/en/stable/api/pytorch_lightning.core.LightningModule.html#pytorch_lightning.core.LightningModule.example_input_array
        # shouldn't declare this before it can be assigned to
        # self.example_input_array: Any = MISSING

    def forward(self, x: Dict[str, torch.Tensor]) -> Dict[str, torch.Tensor]:
        if self.static_variables:
            warnings.warn(
                f'{type(self).__name__} '
                 'handles static="augment" as "propagate"')

        # TODO use packed_sequence
        # 'b t f'

        #
        # get encoder data
        #

        x_data = torch.cat(
                (*(ub.dict_isect(
                    self.input_embeddings(x['encoder_cat']),
                    self.encoder_variables + self.static_variables
                  ).values()),
                 x['encoder_cont']),
            dim=-1)

        #
        # synchronize and get time
        #

        x_time = x['encoder_cont'][:, :, self.t_idx]
        for i in range(1, len(x_time)):
            assert torch.allclose(x_time[0, :2], x_time[i, :2])
        x_time = x_time[x['encoder_lengths'].argmax()]
        y_time = x['decoder_cont'][:, :, self.t_idx]
        for i in range(1, len(y_time)):
            assert torch.allclose(y_time[0, :2], y_time[i, :2])
        y_time = y_time[x['decoder_lengths'].argmax()]

        #
        # get decoder data minus targets (covariates only)
        #

        # make sure targets are at the end of reals for stacking to work
        # categorical targets are not supported
        assert all(self.target_positions == torch.arange(len(self.reals) - len(self.target_positions), len(self.reals), device=self.device))
        y_cov = torch.cat(
                (*(ub.dict_isect(
                    self.input_embeddings(x['decoder_cat']),
                    self.decoder_variables + self.static_variables
                  ).values()),
                 x['decoder_cont'][:, :, :-len(self.target_positions)]),
            dim=-1)

        #
        # feed to network
        #

        y_pred = self.old_forward(y_time, x_data, x_time, y_cov)

        #
        # unnormalize back to orig space
        #

        # has to be a list of tensors...
        y_pred = [*einops.rearrange(y_pred, 'b t f -> f b t 1')]
        y_pred = self.transform_output(y_pred, target_scale=x["target_scale"])
        return self.to_network_output(prediction=y_pred)

    def old_forward(self, y_time, x, x_time, y_cov):
        raise NotImplementedError

    '''
    def training_step(self, batch, batch_idx):
        self.log("my_metric", x)

    The log() method has a few options:

        on_step (logs the metric at that step in training)

        on_epoch (automatically accumulates and logs at the end of the epoch)

        prog_bar (logs to the progress bar)

        logger (logs to the logger like Tensorboard)

    Depending on where the log is called from, Lightning auto-determines the correct mode for you. But of course you can override the default behavior by manually setting the flags.
    '''

    # When using forward, you are responsible to call eval() and use the
    # no_grad() context manager.


class RNNAdapter(PFMixin, pf.RecurrentNetwork):

    def __init__(self, hidden_size=100, **kwargs):

        super().__init__(hidden_size=hidden_size, **kwargs)


class TFTAdapter(PFMixin, pf.TemporalFusionTransformer):

    optuna_best_params = {
        # 'gradient_clip_val': 42.14720820598181,  # trainer
        'hidden_size': 67,
        'dropout': 0.2900708000632789,
        'hidden_continuous_size': 34,
        'attention_head_size': 2,
        'learning_rate': 0.005011872336272725
    }

    # kwargs need to be explicitly in the init args because pf uses the inspect
    # lib to grab them
    def __init__(self,
                 max_encoder_length=100,
                 hidden_size=67,
                 dropout=0.2,
                 hidden_continuous_size=34,
                 attention_head_size=4,
                 learning_rate=1e-2,
                 output_size=1,  # 7,
                 **kwargs):

        super().__init__(
            max_encoder_length=max_encoder_length,
            hidden_size=hidden_size,
            dropout=dropout,
            hidden_continuous_size=hidden_continuous_size,
            attention_head_size=attention_head_size,
            learning_rate=learning_rate,
            **kwargs
        )


class DummyModel(BaseModel):

    def init_xy(self):
        self.net = create_net(self.x_dims, self.y_dims,
                              nonlinear=nn.ReLU).float()

    def old_forward(self, y_time, x, x_time):
        y0 = self.net(x[:, -1, :])
        y = einops.repeat(y0, 'b f -> b t f', t=len(y_time))
        return y


class BaseData(pl.LightningDataModule):
    pass


BASE_FREQ_S = 0.02


@dataclass
class HemorrhageVitals(BaseData):

    read_cache: bool = True

    # dataloader
    batch_size: int = 2  #200
    missing_rate: Optional[float] = None  # 0.7

    # common
    root_path: str = '/data/pulse/hemorrhage/hemorrhage'  # severe dataset
    x_time_minutes: float = 20
    x_start_minutes: float = 0
    total_time_minutes_max: int = 120

    # split_train_val_test
    continue_params: List[str] = field(default_factory=lambda: [
        'HeartRate(1/min)', 'BloodVolume(mL)', 'CardiacOutput(mL/min)',
        'RespirationRate(1/min)', 'OxygenSaturation',
        'MeanArterialPressure(mmHg)', 'SystolicArterialPressure(mmHg)',
        'DiastolicArterialPressure(mmHg)', 'HemoglobinContent(g)',
        'TotalHemorrhageRate(mL/s)', 'TotalHemorrhagedVolume(mL)'
    ])
    # another meaningful difference from google stock: t_param is
    # per-timeseries (per group/pt), so don't have to avoid peeking ahead for val
    t_param: str = 'Time(s)'  # assume seconds

    # cut_and_split_data
    stride: int = 1
    train_fraq: float = 0.7
    val_fraq: Optional[float] = 0.15
    max_pts: Optional[int] = None

    # TRANS: Two kinds of architecture, using AutoEncoder need a small amount
    # of x reverse solving until t0
    # if arch in { 'Seq2Seq', 'VAE' }
    start_reverse: bool = False

    static_behavior: Literal['ignore', 'propagate', 'augment'] = 'augment'
    time_augmentation: bool = False
    num_workers: int = 4

    shuffle: bool = True  # turned off for overfit_batches

    def __post_init__(self):
        super().__init__()
        self._prepare_data()
        self._setup()

    @property
    def _cache_dir(self):
        if not self.hparams:
            self.save_hyperparameters(ignore=['read_cache'])
        # hparam_str = ub.hash_data(dict(self.hparams))
        h = str({k:v for k, v in sorted(self.hparams.items())})
        hparam_str = ub.hash_data(h, base='abc')
        return (ub.Path(self.root_path) / 'cache' / str(hparam_str))

    def _prepare_data(self):
        cache_dir = self._cache_dir
        if not (cache_dir.exists() and self.read_cache):

            kwargs, (df_tr, df_va, df_te) = self._build_dsets()

            # save
            cache_dir.ensuredir()
            with open(cache_dir / 'hparams.json', 'w') as f:
                json.dump(self.hparams, f)
            df_tr.to_pickle(cache_dir / 'df_tr.pkl')
            df_va.to_pickle(cache_dir / 'df_va.pkl')
            df_te.to_pickle(cache_dir / 'df_te.pkl')
            with open(cache_dir / 'kwargs.pkl', 'wb') as f:
                pkl.dump(kwargs, f)

    def _setup(self):
        cache_dir = self._cache_dir
        assert cache_dir.exists()

        # load
        self.df_tr = pd.read_pickle(cache_dir / f'df_tr.pkl')
        self.df_va = pd.read_pickle(cache_dir / f'df_va.pkl')
        self.df_te = pd.read_pickle(cache_dir / f'df_te.pkl')
        with open(self._cache_dir / 'kwargs.pkl', 'rb') as f:
            self.kwargs = pkl.load(f)

    def mins_to_ix(self, mins: float) -> int:
        ASSUMED_FREQ_S = BASE_FREQ_S * self.stride
        return int(mins * 60 / ASSUMED_FREQ_S)

    def _build_dsets(
        self
    ) -> Tuple[pd.DataFrame, Tuple[pf.TimeSeriesDataSet, pf.TimeSeriesDataSet,
                                   pf.TimeSeriesDataSet]]:
        # does not assign to self.

        #
        # get patient entries
        #

        self.root_path = ub.Path(self.root_path)
        manifest = json.load(open(self.root_path / 'patient_results.json'))

        pts = []
        for pt in manifest['Patient']:
            csv = self.root_path / pt['OutputBaseFilename'].lstrip(
                '/') / 'HemorrhageResults.csv'
            js = json.load(csv.with_name('patient.json').open())
            # all pts have StartTime == 10s and TriageTime == 60s for now
            static = ub.dict_isect(pt,
                                   ['Age_yr', 'Height_cm', 'Weight_kg', 'BMI'])
            static['Compartment'] = pt['Hemorrhage']['Compartment']
            static['Severity'] = pt['Hemorrhage']['Severity']
            static['Sex'] = js.get('Sex', 'Male')  # TODO report
            _id = js['Name']

            pts.append((csv, static, _id))
        pts = sorted(pts)

        if self.max_pts is not None:
            pts = np.random.choice(pts, (self.max_pts, ), replace=False)
        # multithread this
        print('loading patients...')
        exc = ub.Executor('thread', max_workers=32)

        # categorical dtypes
        Compartment = pd.api.types.CategoricalDtype(
            ['RightArm', 'RightLeg', 'LeftArm',
             'LeftLeg'])  # TODO confirm these
        Sex = pd.api.types.CategoricalDtype(['Male', 'Female'])

        def _job(pt):
            csv, static, _id = pt
            df = pd.read_csv(csv).assign(id=_id, **static)
            df['Sex'] = df['Sex'].astype(Sex)
            df['Compartment'] = df['Compartment'].astype(Compartment)
            df = df[::self.stride]
            return df

        jobs = [exc.submit(_job, pt) for pt in pts]
        dfs = [j.result() for j in jobs]
        df = pd.concat(dfs, ignore_index=True)
        print(f'found {len(dfs)} patients with {len(df)} timesteps')

        # TODO if not builtin to pf, use
        # https://scikit-learn.org/stable/modules/generated/sklearn.preprocessing.LabelEncoder.html
        # for static_unknown (end time w/o survival analysis)

        # TRANS: The autoregressive prediction, Autoencoder reverse
        # solution to h0 first, then forward solution to the last point
        if self.start_reverse:
            raise NotImplementedError

        #
        # handle sampling
        #

        # don't know why dropna() is necessary here...
        periods = df.groupby(
            ['id'])[self.t_param].diff()[1:-1].dropna().round(6).unique()
        assert len(periods) == 1, periods
        assert (BASE_FREQ_S * self.stride) == periods[0]  # >= 0.02s
        x_points = self.mins_to_ix(self.x_time_minutes)
        y_points = self.mins_to_ix(
                       self.total_time_minutes_max - self.x_time_minutes)
        print(f'{x_points} in timesteps, [1, {y_points}] out timesteps')
        # make t_param integer
        assert (BASE_FREQ_S * self.stride) == periods[0]  # >= 0.02s
        df['time_idx'] = (df[self.t_param] / (BASE_FREQ_S * self.stride)
                         ).astype(int)
        # make continue_params float
        df[self.continue_params] = df[self.continue_params].astype(float)

        #
        # train-test split
        #

        # pts will be deleted from splits for min_x_len after they're computed,
        # so they may still be too small
        uids = df['id'].unique()
        n = len(uids)
        idx = np.random.permutation(n)
        tr_idx = idx[:int(n * self.train_fraq)]
        if self.val_fraq:
            va_idx = idx[int(n * self.train_fraq):int(n * (self.train_fraq + self.val_fraq))]
            te_idx = idx[int(n * (self.train_fraq + self.val_fraq)):]
        else:
            va_idx = idx[int(n * self.train_fraq):]
            te_idx = va_idx
        # ensure each contains at least a full batch
        assert len(tr_idx) >= self.batch_size
        if not len(va_idx) >= self.batch_size:
            diff = batch_size - len(va_idx)
            tr_idx, va_idx = tr_idx[:-diff], np.concatenate(tr_idx[-diff:], va_idx)
        if not sum(te_idx) >= self.batch_size:
            diff = batch_size - len(te_idx)
            tr_idx, te_idx = tr_idx[:-diff], np.concatenate(tr_idx[-diff:], te_idx)

        df_tr = df[df.id.isin(uids[tr_idx])]
        df_va = df[df.id.isin(uids[va_idx])]
        df_te = df[df.id.isin(uids[te_idx])]
        print('train, val, test timesteps: ', df_tr.shape, df_va.shape,
              df_te.shape)

        #
        # build datasets
        #

        '''
        static_cols = [
            'Age_yr',
            'Height_cm',
            'Weight_kg',
            'BMI',
            'Severity',
            'Compartment',
            'Sex',
        ]
        '''
        if self.static_behavior == 'ignore':
            time_varying_known_reals = []
            time_varying_known_categoricals = []
            static_categoricals = []
            static_reals = []
        elif self.static_behavior == 'propagate':
            time_varying_known_reals = [
                'Age_yr',
                'Height_cm',
                'Weight_kg',
                'BMI',
                'Severity',
            ]
            time_varying_known_categoricals = ['Compartment', 'Sex']
            static_categoricals = []
            static_reals = []
        elif self.static_behavior == 'augment':
            time_varying_known_reals = []
            time_varying_known_categoricals = []
            static_reals = [
                'Age_yr',
                'Height_cm',
                'Weight_kg',
                'BMI',
                'Severity',
            ]
            static_categoricals = ['Compartment', 'Sex']

        # TODO add lag if it's not already here

        # change this to GroupNormalizer (on encoder values only?) when
        # adding sliding-time-window data augmentation ie multiple
        # samples per group
        # what does: (on which overfitting tests will fail) mean?

        # have to fit normalizers outside of dset
        # https://github.com/jdb78/pytorch-forecasting/issues/605
        # categorical_encoders={
        # 'product_id': NaNLabelEncoder().fit(df.product_id),
        # 'shop_id': NaNLabelEncoder().fit(df.shop_id),
        # 'product_type': NaNLabelEncoder().fit(df.product_type)
        # },

        target_normalizer = pf.MultiNormalizer(
            [pf.EncoderNormalizer(transformation='softmax')] *
            len(self.continue_params))
        kwargs = dict(
            time_idx='time_idx',
            # mutitarget returns a list because the targets may have different dtypes. How to get around this besides fixing in dataloader? Multiindex? pd.Series of vectors?
            target=self.continue_params,
            group_ids=['id'],
            max_encoder_length=x_points,
            min_prediction_idx=self.mins_to_ix(self.x_time_minutes + self.x_start_minutes),
            min_prediction_length=1,
            max_prediction_length=
            y_points,  # does this have to be masked or truncated?
            # this is duped w/ different normalizer between target and cont
            # worth doing because target doesn't come normalized in dataloader
            time_varying_unknown_reals=self.continue_params,
            time_varying_known_reals=([self.t_param] + time_varying_known_reals),
            time_varying_unknown_categoricals=[],
            time_varying_known_categoricals=time_varying_known_categoricals,
            static_categoricals=static_categoricals,
            static_reals=static_reals,
            add_relative_time_idx=False,
            # this means add to static features; already an entry in x
            # add_target_scales=True,  # TODO figure out how to add static feats
            add_target_scales=False,
            add_encoder_length=False,
            # target_normalizer=target_normalizer,
            target_normalizer='auto',
            scalers={self.t_param: None},
            predict_mode=True,
            # if not randomized, this happens; dec_len = minimax over batch samples
            # decoder_length = self.calculate_decoder_length(time[-1], sequence_length)
            # encoder_length = sequence_length - decoder_length
            # turn this on for sliding-time-window
            randomize_length=False,
        )
        if not self.time_augmentation:
            kwargs.update(
            )
        else:
            raise NotImplementedError

        # TODO missingness

        # TODO y_delay (per batch), y_dim_list (?)

        # print('train, val, test pts after time filtering: ',
              # len(dset_tr.decoded_index.id.unique()),
              # len(dset_va.decoded_index.id.unique()),
              # len(dset_te.decoded_index.id.unique()),
        # )

        return kwargs, (df_tr, df_va, df_te)

    # @functools.cache  # can't be used with dataclass(frozen=False)
    # this'd get heavy for many pts or very small stride, seems ok for now
    # could use shelve module to avoid dealing w serialization
    def _dset(self, df_name, xt_mins: Optional[Tuple[int, int]]=None, **kwargs):

        # # TODO file issue
        # # for sklearn.preprocessing.StandardScaler
        # import sklearn
        # from sklearn.preprocessing import StandardScaler
        # ub.util_hash._HASHABLE_EXTENSIONS.register(StandardScaler)(hash)
        # cache_fpath = (self._cache_dir /
                       # f'{df_name}_{xt_mins}_{ub.hash_data(kwargs)}.pt')
        # HACK, beware, clear cache when testing scaling
        kwargs_save = kwargs.copy()
        kwargs_save.pop('scalers')  # default StandardSCaler() added
        dct = str({k:v for k, v in sorted(kwargs_save.items())})
        dct_hash = ub.hash_data(dct, base='abc')
        cache_fpath = (self._cache_dir /
                       f'{df_name}_{xt_mins}_{dct_hash}.pt')
        if not cache_fpath.exists():

            df = getattr(self, df_name)
            if xt_mins is not None:
                xt_start_ix, xt_end_ix = map(self.mins_to_ix, xt_mins)
                xt_end_ix -= xt_start_ix
                kwargs['max_encoder_length'] = xt_end_ix
                min_pred_idx_diff = (xt_start_ix + xt_end_ix - kwargs['min_prediction_idx'])
                kwargs['min_prediction_idx'] += min_pred_idx_diff
                kwargs['max_prediction_length'] -= min_pred_idx_diff
            dset = pf.TimeSeriesDataSet(df, **kwargs)
            print('loaded ', len(dset.decoded_index.id.unique()), ' pts')
            dset.save(cache_fpath)

        dset = pf.TimeSeriesDataSet.load(cache_fpath)
        return dset

    def _dataloader(self, dset_nm: str,
                    xt_mins: Optional[Tuple[float, float]]=None,
                    xt_restrict=True,
                    train=False):
        # can remove batch_sampler='synchronized' with irregular/masking support
        # (and should write a custom sampler - "TimeBounded"? - to ensure there
        # is some temporal overlap between batch entries)
        # (and that lengths are similar so that not too much is cut off)

        # for speedup, try:
        # dataloader = AsynchronousLoader(DataLoader(ds, batch_size=16), device=device)
        # pl_bolts.datamodules.async_dataloader.AsynchronousLoader
        kwargs = self.kwargs
        if self.time_augmentation:
            kwargs['predict_mode'] = False
            kwargs['randomize_length'] = True  # TODO test this
        dset = self._dset(dset_nm, xt_mins, **kwargs)
        if xt_restrict and xt_mins:
            pts_in_orig = self._dset(dset_nm, **kwargs).decoded_index.id.unique()
            print('filtering', len(set(dset.decoded_index.id.unique()) - set(pts_in_orig)), 'pts')
            dset.filter(lambda df: df.id.isin(pts_in_orig), copy=False)
        if train:
            batch_size = self.batch_size
            shuffle = self.shuffle
        else:
            batch_size = self.batch_size
            # batch_size = self.batch_size * 10
            shuffle = False
        dl = dset.to_dataloader(train=train,
                                batch_size=batch_size,
                                batch_sampler='synchronized',
                                num_workers=self.num_workers,
                                shuffle=shuffle,
                                pin_memory=True)
        return dl

    def train_dataloader(self):
        return self._dataloader('df_tr', train=True)

    def val_dataloader(self):
        return self._dataloader('df_va', train=False)

    def test_dataloader(self):
        return self._dataloader('df_te', train=False)

    def predict_dataloader(self):

        def wrap_predict_dl(dl):

            def _wrap_collate(x, y):
                return x

            orig_collate = deepcopy(dl.collate_fn)

            def _collate(*args, **kwargs):
                return _wrap_collate(*orig_collate(*args, **kwargs))

            dl.collate_fn = _collate

            return dl

        dl = self.dset_te.to_dataloader(train=False,
                                        batch_size=self.batch_size * 10,
                                        batch_sampler='synchronized',
                                        num_workers=self.num_workers,
                                        pin_memory=True)
        return wrap_predict_dataloader(dl)


@dataclass
class GoogleStock(BaseData):
    '''
    pytorch-forecasting version.
    '''

    # dataloader
    batch_size: int = 2  #200
    missing_rate: Optional[float] = 0.7

    # common
    csv_path: str = 'data/google_stock/data.csv'
    x_y_points: Optional[int] = 4  #25
    x_points: Optional[int] = 3  #24

    # split_train_val_test
    continue_params: List[str] = field(
        default_factory=lambda:
        ['Open', 'High', 'Low', 'Close', 'Adj Close', 'Volume'])
    parse_dates: List[str] = field(default_factory=lambda: ['Date'])
    t_param: Optional[str] = 'Date'

    # cut_and_split_data
    csv_rel_fpath: Optional[str] = 'data.csv'
    stride: int = 1
    train_fraq: float = 0.7
    val_fraq: Optional[float] = 0.15
    shuffle: bool = True
    n_start: int = 0

    # TRANS: Two kinds of architecture, using AutoEncoder need a small amount
    # of x reverse solving until t0
    # if arch in { 'Seq2Seq', 'VAE' }
    start_reverse: bool = False

    def __post_init__(self):
        self.csv_path = ub.Path(self.csv_path)

    # ENTRYPOINT
    def setup(self, stage: Optional[str] = None):

        # TRANS: Replace the missing value:
        # https://blog.csdn.net/qq_18351157/article/details/104993254,
        # here to get rid of the missing value corresponding to the point
        df = pd.read_csv(self.csv_path, header=0).dropna(how='any')

        # TRANS: Converts time variable to a floating point number
        if self.parse_dates:

            pd_timestamp = df[self.parse_dates].apply(
                lambda x: pd.to_datetime(x, infer_datetime_format=True))
            pd_timestamp = ((pd_timestamp - pd_timestamp.iloc[0]) //
                            pd.Timedelta(1, 'd')).fillna(0).astype(int)
            df[self.parse_dates] = pd_timestamp

        #TRANS: Add the time sequence x
        # for col in self.parse_dates:
        # df[col + '2'] = df[col]
        # time_varying_unknown_reals.append(col + '2')

        #TRANS: TODO: data processing tag, add it to the scalar?
        # https://blog.csdn.net/qq_32863549/article/details/106972347
        # https://scikit-learn.org/stable/modules/generated/sklearn.preprocessing.LabelEncoder.html?highlight=labelencoder#sklearn.preprocessing.LabelEncoder
        # TODO label_dict?
        # if self.discrete_params is not None:  #TRANS: To deal with tags inside the CSV data, such as wind, finally to merge into the tensor_data, in addition to save a label map?
        # pd_discrete_data = origin_pd_data[self.discrete_params]

        if self.x_y_points is None:
            self.x_y_points = len(df) // 100
        # df['group'] = np.repeat(range(np.ceil(len(df) / x_y_points)), x_y_points)
        df['group'] = 1

        df = df[self.n_start::self.stride]

        # we don't want to impute missing timesteps, so ignore pf's time_idx
        # support and just pretend it's a feature
        df['time_idx'] = df.index

        # TRANS: The autoregressive prediction, Autoencoder reverse
        # solution to h0 first, then forward solution to the last point
        if self.start_reverse:
            raise NotImplementedError
            #TRANS: The linear small
            # x_data_until = split_dict['x_data'][:, :1, :] - 1 / 10 * (
            # split_dict['x_data'][:, 1:2, :] -
            # split_dict['x_data'][:, :1, :])
            # x_time_until = split_dict['x_time'][:, :1, :] - 1 / 10 * (
            # split_dict['x_time'][:, 1:2, :] -
            # split_dict['x_time'][:, :1, :])
            # split_dict['x_data'] = torch.cat(
            # (split_dict['x_data'].flip(dims=[1]), x_data_until), dim=1)
            # split_dict['x_time'] = torch.cat(
            # (split_dict['x_time'].flip(dims=[1]), x_time_until), dim=1)

        # unused
        # train_point = int(len(df) * self.train_fraq)
        # val_point = int(len(df) * (self.val_fraq + self.train_fraq))

        tr_cutoff = int((df[self.t_param].max() - df[self.t_param].min()) *
                        self.train_fraq)
        va_cutoff = int((df[self.t_param].max() - df[self.t_param].min()) *
                        (self.train_fraq + self.val_fraq))
        df_tr = df[lambda x: x[self.t_param] <= tr_cutoff]
        df_va = df[lambda x: x[self.t_param] <= va_cutoff]

        if self.x_points is not None:
            x_points = min(self.x_points, self.x_y_points - 1)
            x_points_tr, x_points_va, x_points_te = [x_points] * 3
        else:
            # TRANS: Regression: 1/2 relationship of default in accordance
            # with the observation point (input) and the observation point
            # (output)
            x_points_tr = len(df_tr) // 2
            x_points_va = len(df_va) // 2
            x_points_te = len(df) // 2

        y_points = self.x_y_points - x_points

        self.dset_tr = TimeSeriesDataSet(
            df_tr,
            # time_idx=self.t_param,
            time_idx='time_idx',
            target=self.continue_params,
            group_ids=['group'],
            min_encoder_length=x_points_tr,
            max_encoder_length=x_points_tr,
            min_prediction_length=y_points,
            max_prediction_length=y_points,
            time_varying_unknown_reals=self.continue_params,
            time_varying_known_reals=self.parse_dates,
            time_varying_unknown_categoricals=[],
            time_varying_known_categoricals=[],
            static_categoricals=[],
            static_reals=[],
            add_relative_time_idx=True,
            # add_target_scales=True,  # TODO figure out how to add static feats
            add_target_scales=False,
            add_encoder_length=False,
            target_normalizer=pf.data.MultiNormalizer(
                [pf.data.EncoderNormalizer(transformation=None)] *
                len(self.continue_params)))

        self.dset_va = TimeSeriesDataSet.from_dataset(
            self.dset_tr,
            data=df_va,
            predict=True,
            stop_randomization=True,
            min_encoder_length=x_points_va,
            max_encoder_length=x_points_va)

        self.dset_te = TimeSeriesDataSet.from_dataset(
            self.dset_tr,
            data=df,
            predict=True,
            stop_randomization=True,
            min_encoder_length=x_points_te,
            max_encoder_length=x_points_te)

        # TODO missingness

        # TODO y_delay (per batch), y_dim_list (?)

    def train_dataloader(self):
        return self.dset_tr.to_dataloader(
            train=True,
            batch_size=self.batch_size,
            # batch_sampler='synchronized',
            num_workers=4)

    def val_dataloader(self):
        return self.dset_va.to_dataloader(
            train=False,
            batch_size=self.batch_size * 10,
            # batch_sampler='synchronized',
            num_workers=4)

    def test_dataloader(self):
        return self.dset_te.to_dataloader(
            train=False,
            batch_size=self.batch_size * 10,
            # batch_sampler='synchronized',
            num_workers=4)

    def predict_dataloader(self):
        return self.dset_te.to_dataloader(train=False,
                                          batch_size=self.batch_size * 10,
                                          batch_sampler='synchronized',
                                          num_workers=4)


def create_net(input_dims,
               output_dims=None,
               n_units=100,
               hidden_layers=0,
               nonlinear=nn.Tanh,
               return_dim=False,
               init=True):
    if output_dims is None:
        output_dims = input_dims

    net = nn.Sequential(
        nn.Linear(input_dims, n_units),
        *([nonlinear(), nn.Linear(n_units, n_units)] * hidden_layers),
        nonlinear(), nn.Linear(n_units, output_dims))

    if init:
        init_network_weights(net)

    if return_dim:
        return net, input_dims, output_dims
    else:
        return net


def split_last_dim(data):
    # TRANS: Bisect tensor last dimension, in order to distinguish between the
    # mean and STD
    last_dim = data.shape[-1]
    last_dim = last_dim // 2
    return data[..., :last_dim], data[..., last_dim:]


def init_network_weights(net, std=0.1):
    #TRANS: Initialize the gaussian distribution
    #TRANS: TODO: if use Xavier initialization?
    for m in net.modules():
        if isinstance(m, nn.Linear):
            nn.init.normal_(m.weight, mean=0, std=std)
            nn.init.constant_(m.bias, val=0)


def sample_standard_gaussian(mu, sigma):
    device = get_device(mu)

    d = torch.distributions.normal.Normal(
        torch.Tensor([0.]).to(device),
        torch.Tensor([1.]).to(device))
    r = d.sample(mu.shape).squeeze(-1)
    return r * sigma.float() + mu.float()


def linspace_vector(start, end, n_points, device=torch.device('cpu')):
    assert start.shape == end.shape

    if len(start.shape) == 0:
        # start and end are scalars
        return torch.linspace(start, end, n_points, device=device)
    if len(start.shape) == 1:
        # start and end are vectors
        return torch.stack([
            torch.linspace(s, e, n_points, device=device)
            for s, e in zip(start, end)
        ],
                           dim=0).T
    raise NotImplementedError


def normalize_data_minmax(data):
    #TRANS: Auxiliary processing data
    data_min = torch.min(data, 0).values
    data_max = torch.max(data, 0).values
    numerator = data - data_min
    denominator = data_max - data_min
    norm_data = numerator / (denominator + 1e-7
                             )  #TRANS: Join the decimal avoid divided by zero
    return norm_data, data_min, data_max
