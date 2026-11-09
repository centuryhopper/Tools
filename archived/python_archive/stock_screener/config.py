"""
Default configuration for the ML Stock Screener.
Edit DEFAULT_TICKERS or pass your own list via the CLI (--tickers / --tickers-file).
"""

# A reasonably diverse starter watchlist. Replace with whatever you actually track.
DEFAULT_TICKERS = [
    "AAPL", "MSFT", "GOOGL", "AMZN", "NVDA", "META", "TSLA",
    "AVGO", "JPM", "V", "UNH", "XOM", "HD", "PG", "COST",
    "AMD", "NFLX", "CRM", "ADBE", "PEP",
]

# How many years of daily history to pull per ticker for training.
LOOKBACK_YEARS = 5

# The model predicts: "will this stock's price be up by TARGET_RETURN or more,
# HORIZON_DAYS trading days from now?" This is a classification target, not a
# guarantee -- it's the question the model is trained to answer.
HORIZON_DAYS = 10
TARGET_RETURN = 0.05  # 5%

# Minimum rows of history required for a ticker to be included (avoids IPOs
# with too little data to build reliable features).
MIN_HISTORY_ROWS = 300

RANDOM_STATE = 42
