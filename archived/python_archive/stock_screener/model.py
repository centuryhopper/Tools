"""
Trains a single classifier on pooled historical data across all tickers
(more training examples, one shared model) and uses it to score today's
data for each ticker.

The label is defined in feature_engineering.add_label(): "did the stock
rise by at least TARGET_RETURN over the next HORIZON_DAYS trading days?"
The model outputs a probability of that happening for each ticker's most
recent row -- that probability is the "score" used to rank stocks.

IMPORTANT: this is a supervised-learning exercise on noisy financial data.
Treat the output as one more input to your own research, not a signal to
act on by itself. See the honest accuracy numbers this script prints
before trusting anything it ranks highly.
"""
from __future__ import annotations

import numpy as np
import pandas as pd
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, precision_score, roc_auc_score

from feature_engineering import FEATURE_COLUMNS


def _pool(feature_frames: dict[str, pd.DataFrame]) -> pd.DataFrame:
    """Stack every ticker's feature dataframe into one long dataframe with a 'ticker' column."""
    frames = []
    for ticker, df in feature_frames.items():
        f = df.copy()
        f["ticker"] = ticker
        f["date"] = f.index
        frames.append(f)
    pooled = pd.concat(frames, axis=0, ignore_index=True)
    return pooled


def train_and_score(
    feature_frames: dict[str, pd.DataFrame],
    random_state: int = 42,
    test_frac: float = 0.2,
    n_estimators: int = 400,
    max_depth: int = 6,
) -> tuple[pd.DataFrame, dict, RandomForestClassifier]:
    """
    Returns (scored_df, metrics, fitted_model).

    scored_df: one row per ticker with its predicted probability of hitting
               the target return, plus the raw feature values used.
    metrics:   honest out-of-sample accuracy/precision/AUC from a time-based
               train/test split (train on the past, test on the more recent
               past -- never on the row being scored today).
    """
    pooled = _pool(feature_frames)

    # Rows usable for training/testing: complete features AND a known label.
    labeled = pooled.dropna(subset=FEATURE_COLUMNS + ["label"]).copy()
    labeled = labeled.sort_values("date")

    if labeled.empty:
        raise ValueError("No labeled rows available -- check your date range / horizon settings.")

    # Time-based split (not random) so the test set is strictly later in time
    # than the training set. This avoids leaking future information and gives
    # a more honest read on how the model would've performed walking forward.
    split_idx = int(len(labeled) * (1 - test_frac))
    split_date = labeled.iloc[split_idx]["date"]
    train = labeled[labeled["date"] < split_date]
    test = labeled[labeled["date"] >= split_date]

    X_train, y_train = train[FEATURE_COLUMNS], train["label"]
    X_test, y_test = test[FEATURE_COLUMNS], test["label"]

    clf = RandomForestClassifier(
        n_estimators=n_estimators,
        max_depth=max_depth,
        min_samples_leaf=20,
        class_weight="balanced",
        random_state=random_state,
        n_jobs=-1,
    )
    clf.fit(X_train, y_train)

    metrics = {"train_rows": len(train), "test_rows": len(test), "positive_rate": float(y_train.mean())}
    if len(test) > 0 and y_test.nunique() > 1:
        y_pred = clf.predict(X_test)
        y_proba = clf.predict_proba(X_test)[:, 1]
        metrics["test_accuracy"] = float(accuracy_score(y_test, y_pred))
        metrics["test_precision"] = float(precision_score(y_test, y_pred, zero_division=0))
        metrics["test_auc"] = float(roc_auc_score(y_test, y_proba))
        metrics["baseline_positive_rate"] = float(y_test.mean())
    else:
        metrics["test_accuracy"] = metrics["test_precision"] = metrics["test_auc"] = None

    # Refit on ALL labeled data (train + test) for the final scoring model --
    # more data in, better final model, now that we've already measured
    # honest out-of-sample performance above.
    clf_final = RandomForestClassifier(
        n_estimators=n_estimators,
        max_depth=max_depth,
        min_samples_leaf=20,
        class_weight="balanced",
        random_state=random_state,
        n_jobs=-1,
    )
    clf_final.fit(labeled[FEATURE_COLUMNS], labeled["label"])

    # Score the most recent available row per ticker (label is NaN there --
    # that's the future we don't know yet, i.e. exactly what we want to predict).
    latest_rows = []
    for ticker, df in feature_frames.items():
        recent = df.dropna(subset=FEATURE_COLUMNS)
        if recent.empty:
            continue
        row = recent.iloc[[-1]].copy()
        row["ticker"] = ticker
        row["date"] = row.index
        latest_rows.append(row)

    latest = pd.concat(latest_rows, axis=0, ignore_index=True)
    proba = clf_final.predict_proba(latest[FEATURE_COLUMNS])[:, 1]
    latest["score"] = proba
    latest = latest.sort_values("score", ascending=False).reset_index(drop=True)

    importances = pd.Series(clf_final.feature_importances_, index=FEATURE_COLUMNS).sort_values(ascending=False)
    metrics["top_features"] = importances.head(8).round(4).to_dict()

    return latest, metrics, clf_final
