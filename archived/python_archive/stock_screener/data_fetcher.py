"""
Pulls historical daily OHLCV data for a list of tickers using yfinance.
"""
from __future__ import annotations

import time
import pandas as pd

try:
    import yfinance as yf
except ImportError as e:
    raise ImportError(
        "yfinance is required. Install it with: pip install yfinance"
    ) from e


def fetch_history(ticker: str, years: int = 5, retries: int = 2, pause: float = 1.5) -> pd.DataFrame | None:
    """
    Download daily OHLCV history for a single ticker.
    Returns None (and prints a warning) if the download fails or comes back empty.
    """
    period = f"{years}y"
    last_err = None
    for attempt in range(retries + 1):
        try:
            df = yf.Ticker(ticker).history(period=period, interval="1d", auto_adjust=True)
            if df is None or df.empty:
                raise ValueError("empty dataframe returned")
            df = df.rename(columns=str.lower)
            df.index.name = "date"
            return df[["open", "high", "low", "close", "volume"]]
        except Exception as e:  # noqa: BLE001 - we want to retry on anything and report it
            last_err = e
            if attempt < retries:
                time.sleep(pause)
    print(f"  [warn] could not fetch {ticker}: {last_err}")
    return None


def fetch_all(tickers: list[str], years: int = 5, min_rows: int = 300) -> dict[str, pd.DataFrame]:
    """
    Fetch history for every ticker in the list. Skips (with a printed warning)
    any ticker that fails to download or doesn't have enough history.
    """
    out: dict[str, pd.DataFrame] = {}
    print(f"Fetching {years}y of daily history for {len(tickers)} tickers...")
    for i, t in enumerate(tickers, 1):
        print(f"  ({i}/{len(tickers)}) {t}")
        df = fetch_history(t, years=years)
        if df is None:
            continue
        if len(df) < min_rows:
            print(f"  [skip] {t}: only {len(df)} rows of history (< {min_rows})")
            continue
        out[t] = df
    print(f"Got usable data for {len(out)}/{len(tickers)} tickers.\n")
    return out


def fetch_sector_info(tickers: list[str]) -> dict[str, dict]:
    """
    Best-effort fetch of sector/industry/market-cap metadata for context in the report.
    Not critical to the model -- failures here are silent per-ticker.
    """
    info = {}
    for t in tickers:
        try:
            raw = yf.Ticker(t).info
            info[t] = {
                "sector": raw.get("sector", "n/a"),
                "industry": raw.get("industry", "n/a"),
                "marketCap": raw.get("marketCap"),
                "shortName": raw.get("shortName", t),
            }
        except Exception:  # noqa: BLE001
            info[t] = {"sector": "n/a", "industry": "n/a", "marketCap": None, "shortName": t}
    return info
