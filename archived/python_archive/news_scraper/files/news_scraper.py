import os
import pandas as pd
from datetime import datetime
from dotenv import load_dotenv
from concurrent.futures import ThreadPoolExecutor, as_completed
from newsapi import NewsApiClient

# Load env vars
load_dotenv("secrets.env")
API_KEY = os.getenv("apiKey")

# Init client
newsapi = NewsApiClient(api_key=API_KEY)

IS_DOCKER = os.getenv("DOTNET_RUNNING_IN_CONTAINER") == "true"
TARGET_DIR = "/app/news" if IS_DOCKER else "./news"
os.makedirs(TARGET_DIR, exist_ok=True)

CATEGORIES = [
    "business",
    "entertainment",
    "health",
    "science",
    "sports",
    "technology"
]


def get_news(category: str):
    response = newsapi.get_top_headlines(
        category=category,
        language="en",
        country="us"
    )

    articles = []

    if response.get("status") == "ok":
        for article in response.get("articles", []):
            articles.append({
                "Title": article.get("title"),
                "Author": article.get("author"),
                "Desc": article.get("description"),
                "Url": article.get("url"),
                "PublishedAt": (
                    article.get("publishedAt", "")[:10]
                    if article.get("publishedAt")
                    else None
                )
            })

    return category, articles


# ----------------------------
# Parallel fetch
# ----------------------------
results = {}

with ThreadPoolExecutor(max_workers=6) as executor:
    futures = [executor.submit(get_news, cat) for cat in CATEGORIES]

    for future in as_completed(futures):
        category, data = future.result()
        results[category] = data


# ----------------------------
# Excel output (multi-sheet)
# ----------------------------
file_name = datetime.now().strftime("%Y_%m_%d") + ".xlsx"
file_path = os.path.join(TARGET_DIR, file_name)

if not os.path.exists(file_path):
    with pd.ExcelWriter(file_path, engine="openpyxl") as writer:
        for category, articles in results.items():
            df = pd.DataFrame(articles)
            df.to_excel(writer, sheet_name=category, index=False)

    print(f"Saved: {file_path}")
else:
    print(f"{datetime.now().date()}: Already retrieved today's news")