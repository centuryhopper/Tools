#!/usr/bin/env python3
"""
ML Stock Screener -- CLI entry point.

This is a research/screening aid, NOT a trading bot. It never places any
orders. It downloads historical price data, trains a classifier to rank
how likely each stock is to rise by a given amount within a given window,
and prints/saves a ranked table for you to research further yourself.

Examples:
    python main.py
    python main.py --tickers AAPL MSFT NVDA TSLA
    python main.py --tickers-file my_watchlist.txt --horizon-days 20 --target-return 0.08
    python main.py --top-n 25 --output results.csv --no-sectors
"""
from __future__ import annotations

import argparse
import sys

import config
from screener import run_screen


def _read_tickers_file(path: str) -> list[str]:
    with open(path) as f:
        return [line.strip().upper() for line in f if line.strip() and not line.startswith("#")]


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(description="ML-assisted stock screener (research aid, not a trading bot).")
    p.add_argument("--tickers", nargs="+", help="Space-separated list of tickers, e.g. AAPL MSFT NVDA")
    p.add_argument("--tickers-file", type=str, help="Path to a text file with one ticker per line")
    p.add_argument("--lookback-years", type=int, default=config.LOOKBACK_YEARS,
                    help=f"Years of history to train on (default {config.LOOKBACK_YEARS})")
    p.add_argument("--horizon-days", type=int, default=config.HORIZON_DAYS,
                    help=f"Trading days ahead to predict (default {config.HORIZON_DAYS})")
    p.add_argument("--target-return", type=float, default=config.TARGET_RETURN,
                    help=f"Required forward return to count as 'up', e.g. 0.05 for 5%% (default {config.TARGET_RETURN})")
    p.add_argument("--top-n", type=int, default=15, help="How many top-ranked tickers to display (default 15)")
    p.add_argument("--output", type=str, default=None, help="Optional path to save the full report as CSV")
    p.add_argument("--no-sectors", action="store_true", help="Skip fetching sector/industry metadata (faster)")
    return p.parse_args()


def main() -> int:
    args = parse_args()

    tickers = None
    if args.tickers and args.tickers_file:
        print("Pass either --tickers or --tickers-file, not both.", file=sys.stderr)
        return 1
    if args.tickers:
        tickers = [t.upper() for t in args.tickers]
    elif args.tickers_file:
        tickers = _read_tickers_file(args.tickers_file)

    print("=" * 70)
    print("ML STOCK SCREENER -- research aid only. Does not place trades.")
    print("Nothing here is financial advice; verify everything yourself")
    print("before making any investment decision.")
    print("=" * 70 + "\n")

    report = run_screen(
        tickers=tickers,
        lookback_years=args.lookback_years,
        horizon_days=args.horizon_days,
        target_return=args.target_return,
        top_n=args.top_n,
        fetch_sectors=not args.no_sectors,
    )

    if args.output:
        report.to_csv(args.output, index=False)
        print(f"\nFull report ({len(report)} tickers) saved to {args.output}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
