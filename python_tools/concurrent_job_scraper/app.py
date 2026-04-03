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

# --- Fetch links using Playwright ---
async def fetch_links(page: Page, url: str, job_scraper: JobScraper) -> list[str]:
    try:
        await page.goto(url, timeout=30000)
        await page.wait_for_load_state("networkidle")
        links = await job_scraper.get_links(page)
        
        # clear dupes
        links = list(dict.fromkeys(links))
        # print(links)
        # pdb.set_trace()  # optional breakpoint
        return links
    except Exception as e:
        print(f"Error fetching {url}: {e}")
        return []

# --- BFS crawler with true concurrency ---
async def crawl(job_scraper: JobScraper, max_urls=2000, max_concurrent=20) -> List[str]:
    parsed_start = urlparse(job_scraper.start_url)
    base_host = parsed_start.netloc
    print(f"Starting crawl at {job_scraper.start_url} (host: {base_host})")

    seen = set([job_scraper.start_url])
    queue = asyncio.Queue()
    await queue.put(job_scraper.start_url)
    results = []

    async with async_playwright() as p:
        browser = await p.chromium.launch(headless=True)
        context = await browser.new_context()

        async def worker():
            page : Page = await context.new_page()
            while len(seen) < max_urls:
                try:
                    url = await asyncio.wait_for(queue.get(), timeout=1)
                except asyncio.TimeoutError:
                    break  # queue empty, exit

                links = await fetch_links(page, url, job_scraper)
                # print(links)
                for link in links:
                    if not link:
                        continue
                    # print("link: {}, job_scraper.should_visit(link): {}".format(link, job_scraper.should_visit(link)))
                    parsed = urlparse(link)
                    if parsed.netloc != base_host:
                        continue  # skip external links
                    if not job_scraper.should_visit(link):
                        continue
                    if link not in seen:
                        seen.add(link)
                        await queue.put(link)

                if job_scraper.results_filter(url):
                    results.append(url)

        # spawn max_concurrent workers
        workers = [asyncio.create_task(worker()) for _ in range(max_concurrent)]
        await asyncio.gather(*workers)

        await browser.close()
    
    # print('results: {}'.format(results))

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
    # scraper = UCFJobScraper()
    # scraper = PeopleFirstJobScraper()
    # scraper = PublixJobScraper()
    scraper = UniversalOrlandoJobs()
    results = await crawl(
        scraper,
        max_urls=2000,
        max_concurrent=20  # true concurrent workers
    )
    
    # c = collections.Counter(results)
    # print(c)
    
    # results = list(dict.fromkeys(results))  # remove duplicates
    
    filename = f"out/results_{time.strftime('%Y_%m_%d_%H_%M_%S')}.csv"
    save_to_csv(filename, results)

if __name__ == "__main__":
    asyncio.run(main())