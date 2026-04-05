import pdb
from typing import List

from playwright.async_api import Page

from ..templates.job_scraper import JobScraper


class UCFJobScraper(JobScraper):
    def __init__(self):
        super().__init__(
            "https://jobs.ucf.edu/jobs/search",
            "UCF_Jobs"
        )
        
    async def get_links(self, page: Page) -> List[str]:
        return await page.eval_on_selector_all(
            "a",
            "elements => elements.filter(e => e.id.includes('link_job_title') || e.rel === 'next').map(e => e.href)"
        )

    def should_visit(self, url: str) -> bool:
        if "jobs/search" in url:
            return True  # always visit search pages

        keywords =  [
            "analyst",
            "programmer",
            "specialist",
            "data",
            "computer",
            "information",
            "tech",
        ]
        
        is_tech_job = any(keyword.lower() in url.lower() for keyword in keywords)
        # print(f"Filtering {url}: is_tech_job={is_tech_job}")
        
        return is_tech_job
    
    def is_job_page(self, url: str) -> bool:
        return "/jobs/" in url
    
    
    def results_filter(self, url: str) -> bool:
        return "/jobs/search" not in url