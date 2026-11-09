"""
Turns raw OHLCV price data into a feature table for the ML model.
All indicators are computed from scratch with pandas/numpy (no extra
technical-analysis dependency required).
"""
from __future__ import annotations

import numpy as np
import pandas as pd


def _rsi(close: pd.Series, window: int = 14) -> pd.Series:
    delta = close.diff()
    gain = delta.clip(lower=0)
    loss = -delta.clip(upper=0)
    avg_gain = gain.ewm(alpha=1 / window, min_periods=window, adjust=False).mean()
    avg_loss = loss.ewm(alpha=1 / window, min_periods=window, adjust=False).mean()
    rs = avg_gain / avg_loss.replace(0, np.nan)
    rsi = 100 - (100 / (1 + rs))
    return rsi.fillna(50)


def _macd(close: pd.Series, fast: int = 12, slow: int = 26, signal: int = 9):
    ema_fast = close.ewm(span=fast, adjust=False).mean()
    ema_slow = close.ewm(span=slow, adjust=False).mean()
    macd_line = ema_fast - ema_slow
    signal_line = macd_line.ewm(span=signal, adjust=False).mean()
    hist = macd_line - signal_line
    return macd_line, signal_line, hist


def _bollinger_pct_b(close: pd.Series, window: int = 20, num_std: float = 2.0) -> pd.Series:
    sma = close.rolling(window).mean()
    std = close.rolling(window).std()
    upper = sma + num_std * std
    lower = sma - num_std * std
    pct_b = (close - lower) / (upper - lower).replace(0, np.nan)
    return pct_b


def build_features(df: pd.DataFrame) -> pd.DataFrame:
    """
    Given a raw OHLCV dataframe (columns: open, high, low, close, volume),
    return a dataframe of engineered features, one row per trading day.
    """
    out = pd.DataFrame(index=df.index)
    close = df["close"]
    volume = df["volume"]

    # --- Trend / moving averages ---
    for w in (10, 20, 50, 200):
        sma = close.rolling(w).mean()
        out[f"close_over_sma{w}"] = close / sma - 1

    out["sma10_over_sma50"] = close.rolling(10).mean() / close.rolling(50).mean() - 1
    out["sma50_over_sma200"] = close.rolling(50).mean() / close.rolling(200).mean() - 1

    # --- Momentum ---
    for w in (5, 10, 20, 60):
        out[f"return_{w}d"] = close.pct_change(w)

    out["rsi_14"] = _rsi(close, 14)

    macd_line, signal_line, hist = _macd(close)
    out["macd_hist"] = hist
    out["macd_line"] = macd_line

    # --- Volatility ---
    daily_ret = close.pct_change()
    for w in (10, 20, 60):
        out[f"volatility_{w}d"] = daily_ret.rolling(w).std()

    out["bollinger_pct_b"] = _bollinger_pct_b(close)

    # --- Volume ---
    vol_sma20 = volume.rolling(20).mean()
    out["volume_ratio_20d"] = volume / vol_sma20.replace(0, np.nan)
    out["volume_trend_20d"] = volume.rolling(20).mean() / volume.rolling(60).mean() - 1

    # --- Price position relative to 52-week range ---
    high_252 = close.rolling(252, min_periods=60).max()
    low_252 = close.rolling(252, min_periods=60).min()
    out["pct_from_52w_high"] = close / high_252 - 1
    out["pct_off_52w_low"] = close / low_252 - 1

    out["close"] = close  # keep for label construction / reporting, drop before training
    return out


def add_label(features: pd.DataFrame, horizon_days: int, target_return: float) -> pd.DataFrame:
    """
    Adds a binary label column 'label': 1 if close price horizon_days in the
    future is >= target_return above today's close, else 0. The most recent
    `horizon_days` rows will have NaN labels (unknown future) -- those are the
    rows we ultimately want to *predict*, not train on.
    """
    close = features["close"]
    future_close = close.shift(-horizon_days)
    fwd_return = future_close / close - 1
    features = features.copy()
    features["fwd_return"] = fwd_return
    features["label"] = (fwd_return >= target_return).astype("float")
    features.loc[fwd_return.isna(), "label"] = np.nan
    return features


FEATURE_COLUMNS = [
    "close_over_sma10", "close_over_sma20", "close_over_sma50", "close_over_sma200",
    "sma10_over_sma50", "sma50_over_sma200",
    "return_5d", "return_10d", "return_20d", "return_60d",
    "rsi_14", "macd_hist", "macd_line",
    "volatility_10d", "volatility_20d", "volatility_60d",
    "bollinger_pct_b",
    "volume_ratio_20d", "volume_trend_20d",
    "pct_from_52w_high", "pct_off_52w_low",
]
