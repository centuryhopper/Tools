import os
import sys
from concurrent.futures import ThreadPoolExecutor
from datetime import datetime
from pathlib import Path

import pandas as pd
from dotenv import load_dotenv
from newsapi import NewsApiClient

SCRIPT_DIR = Path(__file__).parent

# Locally, read secrets.env next to this script. In Docker the variables come
# from compose's env_file instead (load_dotenv never overrides existing ones).
load_dotenv(SCRIPT_DIR / "secrets.env")

API_KEY = os.getenv("apiKey")
# Where the Excel files go: /data in Docker, ./news next to this script locally.
NEWS_DIR = Path(os.getenv("NEWS_DIR", SCRIPT_DIR / "news"))

CATEGORIES = [
    "business",
    "entertainment",
    "health",
    "science",
    "sports",
    "technology",
]


def get_news(newsapi: NewsApiClient, category: str) -> list[dict]:
    response = newsapi.get_top_headlines(category=category, language="en", country="us")

    # Fail loudly instead of saving an empty sheet, so the next run retries.
    if response.get("status") != "ok":
        raise RuntimeError(f"NewsAPI error for '{category}': {response}")

    return [
        {
            "Title": article.get("title"),
            "Author": article.get("author"),
            "Desc": article.get("description"),
            "Url": article.get("url"),
            "PublishedAt": (article.get("publishedAt") or "")[:10] or None,
        }
        for article in response.get("articles", [])
    ]


def main() -> None:
    file_path = NEWS_DIR / (datetime.now().strftime("%Y_%m_%d") + ".xlsx")

    # Check first, so later runs in the day don't spend API requests.
    if file_path.exists():
        print(f"[{datetime.now():%H:%M}] already retrieved today's news")
        return

    if not API_KEY:
        sys.exit("apiKey is not set (put it in secrets.env)")

    newsapi = NewsApiClient(api_key=API_KEY)
    with ThreadPoolExecutor(max_workers=len(CATEGORIES)) as executor:
        # map keeps the sheets in CATEGORIES order
        results = dict(zip(CATEGORIES, executor.map(lambda c: get_news(newsapi, c), CATEGORIES)))

    NEWS_DIR.mkdir(parents=True, exist_ok=True)
    # Write to a temp file, then rename, so a crash mid-write never leaves a
    # half-written file that would block today's retries.
    tmp_path = file_path.with_suffix(".tmp.xlsx")
    with pd.ExcelWriter(tmp_path, engine="openpyxl") as writer:
        for category, articles in results.items():
            pd.DataFrame(articles).to_excel(writer, sheet_name=category, index=False)
    tmp_path.replace(file_path)

    total = sum(len(a) for a in results.values())
    print(f"[{datetime.now():%H:%M}] saved {file_path.name} ({total} articles)")


if __name__ == "__main__":
    main()
