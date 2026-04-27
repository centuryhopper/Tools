import os
import asyncio
import random
import sqlite3
from datetime import datetime
from typing import List
from dotenv import load_dotenv
from bs4 import BeautifulSoup, Tag
from playwright.async_api import async_playwright
import matplotlib.pyplot as plt
import resend
from enum import Enum
load_dotenv("secrets.env")

resend.api_key = os.getenv('resend_api_key')


class TextFormat(Enum):
    PLAIN = "plain"
    HTML = "html"


FMT = "%Y-%m-%d-%H:%M:%S"


IS_DOCKER = os.getenv("DOTNET_RUNNING_IN_CONTAINER") == "true"

DB_PATH = "/app/prices.db" if IS_DOCKER else "./prices.db"

TIMESTAMP_DIR = (
    "/app/script_execution_records" if IS_DOCKER else "./script_execution_records"
)

os.makedirs(TIMESTAMP_DIR, exist_ok=True)

QUERIES = [
    # ("rove-r2-4k-pro-dashcam", "B0BZRKRBHP", "ROVE R2-4K PRO Dash Cam"),
    ("wilson-evo-basketball-29_5", "B00KXVPN8A", "Wilson Evolution Basketballs"),
]


# 🗃️ INIT DB
def init_db():
    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()

    cur.execute("""
    CREATE TABLE IF NOT EXISTS prices (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        product TEXT,
        price REAL,
        rating TEXT,
        reviews TEXT,
        url TEXT,
        date TEXT
    )
    """)

    conn.commit()
    conn.close()


def insert_records(records: List[str]):
    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()
    
    print('records', records)

    for r in records:
        price = float(r[2].replace("$", ""))

        cur.execute("""
        INSERT INTO prices (date, product, price, rating, reviews, url)
        VALUES (?, ?, ?, ?, ?, ?)
        """, (r[0], r[1], price, r[3], r[4], r[5]))

    conn.commit()
    conn.close()


def get_last_price(product):
    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()

    cur.execute("""
    SELECT price FROM prices
    WHERE product = ?
    ORDER BY id DESC LIMIT 1 OFFSET 1
    """, (product,))

    row = cur.fetchone()
    conn.close()

    return row[0] if row else None


def plot_prices(product):
    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()

    cur.execute("""
    SELECT date, price FROM prices
    WHERE product = ?
    """, (product,))

    rows = cur.fetchall()
    conn.close()

    if not rows:
        print("No data to plot")
        return

    dates = [r[0] for r in rows]
    prices = [r[1] for r in rows]

    plt.figure()
    plt.plot(dates, prices)
    plt.title(product)
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.show()


def random_range(min_val, max_val):
    return random.uniform(min_val, max_val)


def get_formatted_url(asin, page):
    return f"https://www.amazon.com/s?k={asin}&page={page}"
    # return f"https://www.amazon.com/s?k={asin}"


def read_last_timestamp():
    path = os.path.join(TIMESTAMP_DIR, "time_stamp.txt")
    if os.path.exists(path):
        with open(path, "r") as f:
            return f.read().strip()
    return None


def extract(node: Tag, title_desc: str):
    title_section = node.select_one("div[data-cy='title-recipe']")
    if not title_section:
        return None

    image_section = node.select_one("div[data-cy='image-container']")
    if not image_section:
        return None
    
    anchor: Tag | None = image_section.select_one("a.a-link-normal")
    if not anchor:
        return None
    
    print('anchor: ', anchor.get("href", ""))

    desc = title_section.get_text(strip=True)
    # print('desc: ', desc)
    if title_desc not in desc:
        # print('title_desc', title_desc)
        return None

    anchor_href = anchor.get("href", "")
    url = "https://www.amazon.com" + anchor_href if anchor_href else ""

    price = node.select_one("span.a-price span.a-offscreen")
    if not price:
        print('price is none')
        return None

    price_text = price.get_text(strip=True)
    
    # print('price: ', price_text)

    rating = node.select_one('a[aria-label*="out of 5 stars"]')
    rating_text = rating.get("aria-label", "") if rating else ""

    reviews = node.select_one("span.a-size-base.s-underline-text")
    review_text = reviews.get_text(strip=True) if reviews else ""
    
    retVal = (
        datetime.now().strftime("%Y-%m-%d"),
        title_desc,
        price_text,
        rating_text,
        review_text,
        url,
    )
    
    # print(f"Extracted record: {retVal}")

    return retVal


def alert_price_drops(records):
    for r in records:
        product = r[1]
        price = float(r[2].replace("$", ""))

        last_price = get_last_price(product)

        if last_price and price < last_price:
            print(f"🔥 PRICE DROP: {product}")
            print(f"{last_price} → {price}")


SEM = asyncio.Semaphore(4)


async def process_query(browser, query):
    async with SEM:
        last = read_last_timestamp()

        if last:
            last_dt = datetime.strptime(last, FMT)
            if last_dt.date() == datetime.today().date():
                print('already processed today')
                return

        filename, asin, desc = query
        records = []

        context = await browser.new_context(
            viewport={"width": 1920, "height": 1080}
        )
        page = await context.new_page()

        for page_num in range(1,7):
            url = get_formatted_url(asin, page_num)

            await page.goto(url, timeout=60000)
            
            # important to have or else program wont work
            await page.reload()
            
            await page.wait_for_selector("div.s-main-slot", timeout=10000)
            
            html = await page.content()
            soup = BeautifulSoup(html, "html.parser")
            
            results = soup.find_all("div", {"data-component-type": "s-search-result"})

            first_real = None
            search_results = []

            for item in results:
                # Skip sponsored / ads
                if item.get("data-component-id") == "":  
                    continue
                
                # Must have a product link (real item)
                link = item.find("a", class_="a-link-normal s-no-outline")
                if not link:
                    continue

                # first_real = item
                search_results.append(item)
                
            for section in search_results:
                # print(section)
                record = extract(section, desc)
                if record:
                    records.append(record)
                    break

            # print('first_real', first_real)

            await asyncio.sleep(random_range(0.2, 1.2))

        await context.close()

        insert_records(records)
        alert_price_drops(records)
        
        

async def send_email_async(to_email: str, subject: str, body: str, text_format: TextFormat = TextFormat.HTML):
    params: resend.Emails.SendParams = {
        "from": "support@leoums.dev",
        "to": [to_email],
        "subject": subject,
    }

    if text_format == TextFormat.PLAIN:
        params["text"] = body
    elif text_format == TextFormat.HTML:
        params["html"] = body

    response = await resend.Emails.send_async(params)
    return response


async def main():
    init_db()

    async with async_playwright() as p:
        browser = await p.firefox.launch(headless=True)

        tasks = [process_query(browser, query) for query in QUERIES]

        await asyncio.gather(*tasks)

        await browser.close()

    timestamp_path = os.path.join(TIMESTAMP_DIR, "time_stamp.txt")

    with open(timestamp_path, "w") as f:
        f.write(datetime.now().strftime(FMT))




if __name__ == "__main__":
    asyncio.run(main())
    
    
    
# convert db file to csv in linux terminal:
    # sqlite3 -header -csv prices.db "SELECT * FROM prices;" > output.csv