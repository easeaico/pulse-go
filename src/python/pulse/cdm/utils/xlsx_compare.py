# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import argparse
import logging
import numpy as np
import pandas as pd

from openpyxl.reader.excel import load_workbook
from openpyxl.utils import get_column_letter
from openpyxl.workbook import Workbook
from openpyxl.worksheet.table import Table, TableStyleInfo
from pathlib import Path
from pulse.cdm.utils.file_utils import get_root_dir, get_data_dir

_log = logging.getLogger('log')

# Core functionality here pulled from
# https://github.com/pretoriusdre/similarpanda


def find_sheet_diffs(src, dst, key_column: str = None):
    src = src.copy()
    dst = dst.copy()

    def get_row_status(record):
        """ A helper function to create a summary of whether a row has changed.
        Applied to each row in the dataframe"""
        explain = {'left_only': 'Row added', 'right_only': 'Row deleted', 'both': '-'}
        status = explain[record['_merge']]
        if status == '-':
            if len(record['Value changes']) > 0:
                status = 'Row updated'
        return status

    def get_row_changes(record):
        """A helper function to create a dictionary describing the changes in each row.
        Applied to each row in the dataframe"""
        changes_dict = {}
        for col in cols_reduced:

            src_val = None
            if col in src_cols:
                src_val = record[col + '_src']

            if pd.isna(src_val):
                src_val = None
            if record[col] != src_val:
                dst_val = record[col]
                if pd.isna(dst_val):
                    dst_val = None
                if (src_val is not None) or (dst_val is not None):
                    changes_dict[col] = {src_val: dst_val}
        return changes_dict

    dst_cols = list(dst.columns)
    src_cols = list(src.columns)

    added_cols = [col for col in dst_cols if col not in src_cols]
    deleted_cols = [col for col in src_cols if col not in dst_cols]

    #  Exclude the key column, as this will always match
    cols_reduced = [col for col in dst_cols if col != key_column]

    src['row_id'] = np.arange(src.shape[0])
    dst['row_id'] = np.arange(dst.shape[0])

    if key_column is None:
        key_column = 'row_id'

    df_differences = pd.merge(dst, src, on=key_column, suffixes=('', '_src'), how='outer', indicator=True)
    df_differences.drop('row_id', axis=1)
    df_differences['Value changes'] = df_differences.apply(get_row_changes, axis=1)
    df_differences['Row status'] = df_differences.apply(get_row_status, axis=1)
    cols_to_return = dst_cols
    cols_to_return.extend(deleted_cols)
    cols_to_return.extend(['Row status', 'Value changes'])

    return df_differences[cols_to_return], added_cols, deleted_cols


def compare_xlsx_files(src: Path, dst: Path):
    if not src.exists():
        _log.fatal(f"{src} does not exist")
        return
    if not dst.exists():
        _log.fatal(f"{dst} does not exist")
        return

    start_row = 8
    start_col = 2

    differences = Workbook()
    # Remove default sheet(s)
    for s in differences.sheetnames:
        differences.remove(differences[s])

    src_wkbk = load_workbook(filename=src, data_only=True)
    src_sheets = src_wkbk.sheetnames
    dst_wkbk = load_workbook(filename=dst, data_only=True)
    dst_sheets = dst_wkbk.sheetnames
    # Check if the dst has a new sheet
    for s in dst_sheets:
        if s not in src_sheets:
            _log.error(f"dst workbook has sheet {s} that is not in the src workbook")

    for s in src_sheets:
        if s not in dst_sheets:
            _log.error(f"dst workbook does not have a {s} sheet")
            continue

        def fix_up_dataframe(df):
            # Finding diffs assumes all key names are unique, so we need to make sure the key column values unique
            if df.columns[0] == "Output" and "Request Type" in df.columns:
                # System Validation key column is "Output" so make sure those are unique
                #   - Assessments can reuse output names in system validation
                #   - Properties can be duplicated for min and max algorithm validation values
                for idx, row in df.iterrows():
                    prefix = ""
                    suffix = ""
                    name = row["Output"]
                    rq = row["Request Type"]
                    if isinstance(rq, str) and '@' in rq:
                        prefix = rq[:rq.index('@')] + '-'
                    algo = row["Algorithm"]
                    if isinstance(algo, str) and 'Mean' not in algo:
                        suffix = '-' + algo
                    row["Output"] = prefix + name + suffix
            return df
        
        src_df = fix_up_dataframe(pd.read_excel(src, sheet_name=s))
        dst_df = fix_up_dataframe(pd.read_excel(dst, sheet_name=s))
        df_changes, added_cols, deleted_cols = find_sheet_diffs(src_df, dst_df, src_df.columns[0])

        if (len(df_changes[df_changes["Row status"] == "Row updated"]) == 0
                and len(added_cols) == 0 and len(deleted_cols) == 0):
            _log.info(f"{s}: No differences")
            continue  # No changes in this sheet

        diff_sheet = differences.create_sheet(s)
        _log.info(f"{s}: FOUND differences")

        def xlref(row, column, zero_indexed=True):
            if zero_indexed:
                row += 1
                column += 1
            return get_column_letter(column) + str(row)

        cols = df_changes.columns
        for col_index, col in enumerate(cols):
            cell = diff_sheet.cell(column=(col_index + start_col), row=start_row)
            cell.value = col
            if col in added_cols:
                cell.style = 'Good'
            if col in deleted_cols:
                cell.style = 'Bad'

        for row_index, record in df_changes.iterrows():
            for col_index, col in enumerate(cols):
                val_to_print = record[col]
                if type(val_to_print) is dict:
                    val_to_print = str(val_to_print).replace('},', '},\n')
                cell = diff_sheet.cell(column=(col_index + start_col), row=(row_index + start_row + 1))
                cell.value = val_to_print
                if col in record['Value changes'].keys():
                    cell.style = 'Neutral'

                if col == 'Value changes' and len(record['Value changes']) > 0:
                    cell.style = 'Neutral'

                if col in added_cols:
                    cell.style = 'Good'
                if col in deleted_cols:
                    cell.style = 'Bad'

                if record['Row status'] == 'Row added':
                    cell.style = 'Good'
                if record['Row status'] == 'Row deleted':
                    cell.style = 'Bad'
                if cell.value == 'Row updated':
                    cell.style = 'Neutral'

        diff_sheet.cell(column=2, row=2).value = 'Data comparison tool'
        for col_ref in range(2, 6):
            diff_sheet.cell(column=col_ref, row=2).style = 'Headline 1'
        diff_sheet.cell(column=2, row=3).value = 'src:'
        diff_sheet.cell(column=3, row=3).value = str(src)

        diff_sheet.cell(column=2, row=4).value = 'dst:'
        diff_sheet.cell(column=3, row=4).value = str(dst)

        table_style = TableStyleInfo(
            name='TableStyleMedium9',
            showFirstColumn=False,
            showLastColumn=False,
            showRowStripes=True,
            showColumnStripes=False)

        top_left_cell = xlref(start_row, start_col, zero_indexed=False)
        top_right_cell = xlref(start_row + len(df_changes), start_col + len(cols) - 1, zero_indexed=False)
        table_ref = top_left_cell + ':' + top_right_cell
        tab = Table(displayName=f"{s.replace(' ','_')}Differences", ref=table_ref)
        tab.tableStyleInfo = table_style
        diff_sheet.add_table(tab)

    if len(differences.sheetnames) > 0:
        differences.save(filename=dst.parent / (dst.stem + ".diff.xlsx"))


def main():
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

    parser = argparse.ArgumentParser(description="Process the full pipeline for segment validation")
    parser.add_argument(
        "--src",
        type=Path,
        default=None,
        help="Source xlsx filename"
    )
    parser.add_argument(
        "--to",
        type=Path,
        default=None,
        help="xlsx filename to compare to the source xlsx"
    )
    args = parser.parse_args()
    if not args.to.exists():
        _log.fatal(f"--to file does not exist: {args.to}")
        return

    src_xls = None
    if args.src is None:
        def get_basename(path_obj: Path) -> str:
            if not path_obj.suffixes:
                return str(path_obj)  # No extensions to remove
            # Join all suffixes and remove them from the string representation of the path
            all_suffixes = "".join(path_obj.suffixes)
            new_name = str(path_obj).rstrip(all_suffixes)
            return new_name

        def get_file_list(directory, extension):
            matching_files = [f for f in directory.rglob(f"*{extension}") if f.is_file()]
            return matching_files

        # What is the name of what we want to compare to
        xls_basename = get_basename(args.to)
        # Grab all the xlsx files in the source directory
        xls_files = get_file_list(Path(get_root_dir()), ".xlsx")
        for xls_file in xls_files:
            if xls_file.stem == xls_basename:
                src_xls = xls_file
                break
        if src_xls is None:
            _log.fatal(f"Cannot find a file in the source for file: {args.to}")
            return
    else:
        src_xls = args.src

    if not src_xls.exists():
        _log.fatal(f"--src file does not exist: {src_xls}")
        return

    _log.info(f"Comparing {args.to} to {src_xls}")
    compare_xlsx_files(src_xls, args.to)


if __name__ == "__main__":
    main()
