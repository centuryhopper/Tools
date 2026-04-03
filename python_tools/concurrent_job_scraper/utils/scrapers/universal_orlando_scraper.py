import pdb
import re
from typing import List

from playwright.async_api import Page

from ..templates.job_scraper import JobScraper


class UniversalOrlandoJobs(JobScraper):
    def __init__(self):
        super().__init__(
            "https://jobs.universalparks.com/job-search-results/?location=FL%2C%20Orlando",
            "UniversalOrlandoJobs"
        )
        
    async def get_links(self, page: Page) -> List[str]:
        a_tags : List[str] = await page.eval_on_selector_all(
                "a",
                """elements => elements.filter(e => 
                        e.id.includes('job-result')
                    )
                    .map(e => e.href)
                """
            )
        try:
            if page.url == self.start_url:
                onclick_attr = await page.eval_on_selector(
                    "a[aria-label='Go to the last page of results.']",
                    "el => el.getAttribute('onclick')"
                )
                # Regex to find the first number inside the parentheses
                match = re.search(r"goto_page\((\d+),", onclick_attr)
                total_pages = 0
                if match:
                    total_pages = int(match.group(1)) + 1  # Add 1 because page numbers are zero-indexed
                # pdb.set_trace()
                
                # Generate URLs for all pages based on the total number of pages
                for i in range(2, total_pages+1):
                    a_tags.append("https://jobs.universalparks.com/job-search-results/?location=FL%2C%20Orlando&pg={}".format(i))
        except Exception as e:
            pass
        
        # pdb.set_trace()
        
        return a_tags

    def should_visit(self, url: str) -> bool:
        
        # include other pages that list jobs, not just individual job postings
        if "job-search-results" in url:
            return True

        keywords =  [
            "analyst",
            "programmer",
            "specialist",
            "data",
            "computer",
            "information",
            "tech",
            "architect",
        ]

        is_tech_job = any(keyword.lower() in url.lower() for keyword in keywords)
        
        return is_tech_job
    
    
    def results_filter(self, url: str) -> bool:
        return 'job-search-results' not in url