# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging
import numpy as np
import pandas as pd
from pathlib import Path


_log = logging.getLogger("pulse")


def read_csv_into_df(csv_filename: Path, replace_slashes: bool = False, **kwargs):
    df = pd.read_csv(csv_filename, **kwargs)
    for column in df.columns[1:]:
        # Convert any strings to NaN
        df[column] = pd.to_numeric(df[column], downcast='float', errors='coerce')
        # Replace slashes in units string
        if replace_slashes:
            df.rename(columns={column: column.replace("/", "_Per_")}, inplace=True)

    return df


def concat_csv_into_df(earlier: Path, later: Path, replace_slashes: bool = False, **kwargs):
    df1 = read_csv_into_df(earlier, replace_slashes)
    df2 = read_csv_into_df(later, replace_slashes)
    concat_dataframes(df1, df2)


def concat_dataframes(df1: Path, df2: Path):

    # Make sure these dataframes have close overlap
    def overlap(start1, end1, start2, end2):
        if start2 == end1:  # Edge overlap is ok
            return False
        if start1 <= start2 < end1:
            return True
        if start1 <= end2 <= end1:
            return True
        return not (end1 <= start2 or end2 <= start1)

    df1_start = df1["Time(s)"].iloc[0]
    df1_end = df1["Time(s)"].iloc[-1]
    df2_start = df2["Time(s)"].iloc[0]
    df2_end = df2["Time(s)"].iloc[-1]
    if df2_start < df1_start:  # Well, maybe user gave it to us in the wrong order...
        if overlap(df2_start, df2_end, df1_start, df1_end):
            _log.error("CSV files overlap, returning empty dataframe")
            return pd.DataFrame()
        if df2_end == df1_start:
            # Remove the last row
            df2 = df2.drop(df2.index[-1])
        # Concatenate the DataFrames
        return pd.concat([df2, df1], ignore_index=True)

    if overlap(df1_start, df1_end, df2_start, df2_end):
        _log.error("CSV files overlap, returning empty dataframe")
        return pd.DataFrame()
    if df1_end == df2_start:
        # Remove the last row
        df1 = df1.drop(df1.index[-1])
    # Concatenate the DataFrames
    return pd.concat([df1, df2], ignore_index=True)


def compute_means(csv_filename: Path, headers: [str], start_row=0, end_row=-1):
    means = []

    # Load up the csv file, pull the headers, and compute the mean of values from start to end
    df = read_csv_into_df(csv_filename).loc[start_row:end_row]
    means = df[headers].mean().values.tolist()

    return means


def remove_empty_cols(df: pd.DataFrame):
    drop_cols = []
    for c in df.columns:
        if df[c].replace(r'^-1\.\$$', np.nan, regex=True).isna().all():
            drop_cols.append(c)
    if drop_cols:
        return df.drop(columns=drop_cols)
    return df
