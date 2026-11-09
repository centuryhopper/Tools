# ML Stock Screener

A research aid that does the "heavy lifting" of scanning a watchlist for you.
It does **not** place trades, connect to a broker, or manage money — it just
downloads price history, trains a model, and prints a ranked list for you to
look into further.

## What it actually does

1. Downloads N years of daily price history for each ticker (via `yfinance`).
2. Builds ~20 technical features per day: moving averages, RSI, MACD,
   volatility, volume trends, 52-week high/low position, etc.
3. Defines a label from history: *"did this stock rise by at least X% within
   the next Y trading days?"* — this is only knowable in hindsight, so it's
   used to train the model on the past.
4. Trains a Random Forest classifier on that pooled history across all your
   tickers, and reports honest out-of-sample accuracy/precision/AUC using a
   time-based train/test split (it never tests on the same period it trained on).
5. Applies the trained model to **today's** data (where the future is unknown)
   and outputs a probability score per ticker — higher score = the model
   thinks this stock more closely resembles past setups that went on to hit
   the target return.

## Setup

```bash
uv add -r requirements.txt
```

## Usage

```bash
# Default watchlist (config.py), default 5-year lookback, 10-day/5% target
python main.py

# Your own tickers
python main.py --tickers AAPL MSFT NVDA TSLA AMD

# From a file (one ticker per line, # for comments)
python main.py --tickers-file my_watchlist.txt

# Different prediction target: "up 8% within 20 trading days"
python main.py --horizon-days 20 --target-return 0.08

# Save the full ranked report
python main.py --top-n 25 --output results.csv
```

## Reading the output

The script first prints **model validation** — accuracy, precision, and ROC
AUC on a held-out, more-recent slice of data the model never trained on.
**Read this before trusting the rankings.** An AUC near 0.5 means the model
isn't finding meaningful signal for that watchlist/target combo — that's a
real and common outcome with short-term stock prediction, and the honest
answer is "the model isn't confident, look elsewhere for this decision."

Then it prints a ranked table:

| column | meaning |
|---|---|
| `score_%` | Model's estimated probability (0–100) that the stock hits your target return within the horizon |
| `sector` | From Yahoo Finance metadata |
| `close` | Latest close price |
| `rsi_14` | 14-day RSI (>70 often read as overbought, <30 oversold) |
| `20d_return_%` | Trailing 20-trading-day return |
| `pct_from_52w_high_%` | How far below (negative) or above the 52-week high |

## Customizing

- `config.py` — default watchlist, lookback window, horizon/target, minimum history.
- `feature_engineering.py` — add/remove indicators in `FEATURE_COLUMNS`.
- `model.py` — swap in a different classifier (e.g. `GradientBoostingClassifier`,
  `xgboost.XGBClassifier`) if you want to experiment; the interface (`fit`,
  `predict_proba`) is what matters.

## Honest limitations (please read)

- Stock prices are extremely noisy over short horizons. A model that beats a
  coin flip by a little is a realistic outcome, not a bug.
- This uses only price/volume technical features — no fundamentals, no news,
  no macro data. It will miss anything driven by earnings, catalysts, or
  events not reflected in recent price action.
- Past patterns "working" in backtests doesn't guarantee they keep working —
  markets adapt.
- This tool ranks a watchlist you already chose; it doesn't discover new
  ideas or replace due diligence, and it isn't investment advice.
- It never executes trades. Buying/selling is entirely on you, in your own
  brokerage, after your own research.

## Files

```
config.py               default tickers + tunable parameters
data_fetcher.py         pulls OHLCV history + sector metadata from yfinance
feature_engineering.py  computes technical indicators + the training label
model.py                trains the Random Forest, validates it, scores today's data
screener.py             orchestrates the pipeline end-to-end, prints the report
main.py                 CLI entry point
```
