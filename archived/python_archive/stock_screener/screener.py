"""
Orchestrates the full pipeline: fetch data -> build features -> train model
-> score current stocks -> print/save a ranked report.
"""
from __future__ import annotations

import pandas as pd

import config
from data_fetcher import fetch_all, fetch_sector_info
from feature_engineering import build_features, add_label, FEATURE_COLUMNS
from model import train_and_score


def run_screen(
    tickers: list[str] | None = None,
    lookback_years: int = None,
    horizon_days: int = None,
    target_return: float = None,
    top_n: int = 15,
    fetch_sectors: bool = True,
) -> pd.DataFrame:
    tickers = tickers or config.DEFAULT_TICKERS
    lookback_years = lookback_years or config.LOOKBACK_YEARS
    horizon_days = horizon_days or config.HORIZON_DAYS
    target_return = target_return if target_return is not None else config.TARGET_RETURN

    raw = fetch_all(tickers, years=lookback_years, min_rows=config.MIN_HISTORY_ROWS)
    if not raw:
        raise RuntimeError("No data fetched for any ticker -- check your internet connection / ticker symbols.")

    print("Building features and labels...")
    feature_frames = {}
    for ticker, df in raw.items():
        feats = build_features(df)
        feats = add_label(feats, horizon_days=horizon_days, target_return=target_return)
        feature_frames[ticker] = feats

    print("Training model and scoring current data (this may take a moment)...\n")
    scored, metrics, _model = train_and_score(feature_frames, random_state=config.RANDOM_STATE)

    _print_metrics(metrics, horizon_days, target_return)

    sector_info = fetch_sector_info(list(scored["ticker"])) if fetch_sectors else {}
    scored["sector"] = scored["ticker"].map(lambda t: sector_info.get(t, {}).get("sector", "n/a"))

    report_cols = ["ticker", "sector", "score", "close", "rsi_14", "return_20d", "pct_from_52w_high"]
    report = scored[report_cols].copy()
    report["score"] = (report["score"] * 100).round(1)
    report["return_20d"] = (report["return_20d"] * 100).round(1)
    report["pct_from_52w_high"] = (report["pct_from_52w_high"] * 100).round(1)
    report["rsi_14"] = report["rsi_14"].round(1)
    report = report.rename(columns={
        "score": "score_%",
        "return_20d": "20d_return_%",
        "pct_from_52w_high": "pct_from_52w_high_%",
    })

    print(f"Top {top_n} by model score (probability of hitting the "
          f"+{target_return*100:.0f}% target within {horizon_days} trading days):\n")
    print(report.head(top_n).to_string(index=False))

    return report


def _print_metrics(metrics: dict, horizon_days: int, target_return: float) -> None:
    print("=" * 70)
    print("MODEL VALIDATION (out-of-sample, time-based train/test split)")
    print("=" * 70)
    print(f"Target definition: price up >= {target_return*100:.0f}% within {horizon_days} trading days")
    print(f"Training rows: {metrics['train_rows']}   Test rows: {metrics['test_rows']}")
    if metrics.get("test_accuracy") is not None:
        print(f"Test accuracy:  {metrics['test_accuracy']:.3f}")
        print(f"Test precision: {metrics['test_precision']:.3f}  "
              f"(of predicted 'up' calls, fraction that were actually up)")
        print(f"Test ROC AUC:   {metrics['test_auc']:.3f}  (0.5 = coin flip, 1.0 = perfect)")
        print(f"Base rate in test set: {metrics['baseline_positive_rate']:.3f} "
              f"(fraction of all test rows that hit the target anyway)")
    else:
        print("Not enough test data / class variety to compute out-of-sample metrics.")
    print("\nFeatures the model relied on most:")
    for feat, imp in metrics["top_features"].items():
        print(f"  {feat:<22s} {imp:.4f}")
    print("=" * 70)
    print("Read this honestly: if test AUC is close to 0.5, the model isn't finding much\n"
          "signal above chance -- treat rankings as a rough sort, not a forecast.\n")
