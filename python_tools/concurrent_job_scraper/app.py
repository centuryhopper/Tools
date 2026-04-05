import asyncio
import collections
import pdb
import time
from typing import List
from playwright.async_api import Page, async_playwright
from urllib.parse import urlparse
import csv
import os

from utils.scrapers.people_first_scraper import PeopleFirstJobScraper
from utils.scrapers.publix_job_scraper import PublixJobScraper
from utils.scrapers.ucf_job_scraper import UCFJobScraper
from utils.scrapers.universal_orlando_scraper import UniversalOrlandoJobs
from utils.templates.job_scraper import JobScraper



# --- Config ---
MAX_RETRIES = 3
INITIAL_BACKOFF = 0.5


# --- Fetch links with retry ---
async def fetch_links(page: Page, url: str, job_scraper: JobScraper) -> List[str]:
    for attempt in range(MAX_RETRIES):
        try:
            await page.goto(url, timeout=30000)
            await page.wait_for_load_state("networkidle")
            links = await job_scraper.get_links(page)
            return list(dict.fromkeys(links))
        except Exception as e:
            if attempt == MAX_RETRIES - 1:
                print(f"[ERROR] {url}: {e}")
                return []
            await asyncio.sleep(INITIAL_BACKOFF * (2 ** attempt))
    return []


# --- Fully concurrent crawler ---
async def crawl(job_scraper: JobScraper, max_urls=2000, max_concurrent=20) -> List[str]:
    parsed_start = urlparse(job_scraper.start_url)
    base_host = parsed_start.netloc

    print(f"Starting crawl at {job_scraper.start_url} (host: {base_host})")

    seen = set()
    results = []

    seen_lock = asyncio.Lock()
    results_lock = asyncio.Lock()

    queue = asyncio.Queue()
    await queue.put(job_scraper.start_url)
    seen.add(job_scraper.start_url)

    semaphore = asyncio.Semaphore(max_concurrent)

    async with async_playwright() as p:
        browser = await p.chromium.launch(headless=True)
        context = await browser.new_context()

        async def process_url(url: str):
            async with semaphore:
                page = await context.new_page()
                try:
                    links = []
                    if not job_scraper.is_job_page(url):
                        links = await fetch_links(page, url, job_scraper)
                finally:
                    await page.close()

            # Process discovered links
            for link in links:
                if not link:
                    continue

                parsed = urlparse(link)
                if parsed.netloc != base_host:
                    continue

                if not job_scraper.should_visit(link):
                    continue

                async with seen_lock:
                    if len(seen) >= max_urls:
                        break
                    if link in seen:
                        continue
                    seen.add(link)
                    await queue.put(link)

            # Save result
            if job_scraper.results_filter(url):
                async with results_lock:
                    results.append(url)

            queue.task_done()

        tasks = set()

        async def scheduler():
            while True:
                url = await queue.get()
                task = asyncio.create_task(process_url(url))
                tasks.add(task)
                task.add_done_callback(tasks.discard)

        scheduler_task = asyncio.create_task(scheduler())

        await queue.join()  # wait until all tasks are done

        scheduler_task.cancel()
        await browser.close()

    return results

# --- Save to CSV ---
def save_to_csv(filename: str, urls: list[str]):
    os.makedirs("out", exist_ok=True)
    with open(filename, "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f, quoting=csv.QUOTE_ALL)
        writer.writerow(["URL"])
        for url in urls:
            writer.writerow([url])
    print(f"Saved {len(urls)} URLs to {filename}")

# --- Main ---
async def main():
    scrapers : List[JobScraper] = [
        UniversalOrlandoJobs(),
        UCFJobScraper(),
        PeopleFirstJobScraper(),
        PublixJobScraper(),
    ]
    results = []
    for scraper in scrapers:
        results.extend(await crawl(
            scraper,
            max_urls=2000,
            max_concurrent=20)
        )
    
    # c = collections.Counter(results)
    # print(c)
    # results = list(dict.fromkeys(results))  # remove duplicates
    
    filename = f"out/results_{time.strftime('%Y_%m_%d_%H_%M_%S')}.csv"
    save_to_csv(filename, results)

if __name__ == "__main__":
    asyncio.run(main())