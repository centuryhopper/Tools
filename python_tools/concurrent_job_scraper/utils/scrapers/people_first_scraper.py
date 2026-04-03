import pdb
from typing import List

from playwright.async_api import Page

from ..templates.job_scraper import JobScraper


class PeopleFirstJobScraper(JobScraper):
    def __init__(self):
        super().__init__(
            "https://jobs.myflorida.com/go/Science%2C-Technology%2C-Engineering-&-Mathematics/2814200/",
            "PeopleFirst_Jobs"
        )
        
    async def get_links(self, page: Page) -> List[str]:
        return await page.eval_on_selector_all(
            "a",
            """elements => elements.filter(e => 
                e.classList.contains('jobTitle-link') ||
                    (e.title && e.title.includes('Page'))
                )
                .map(e => e.href)
            """
        )

    def should_visit(self, url: str) -> bool:
        if '/job/' not in url and '/go/' not in url:
            return False

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
        
        return is_tech_job
    
    
    def results_filter(self, url: str) -> bool:
        return '#content' not in url and "#" not in url and '/go/' not in url