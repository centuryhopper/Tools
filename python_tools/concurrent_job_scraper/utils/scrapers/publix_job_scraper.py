from typing import List
from playwright.async_api import Page
from ..templates.job_scraper import JobScraper


class PublixJobScraper(JobScraper):
    def __init__(self):
        super().__init__(
            "https://jobs.publix.com/jobs/?p=1&per_page=100&pg=1&department_type_slug=publix-technology",
            "Publix_Jobs"
        )
        
    async def get_links(self, page: Page) -> List[str]:
        return await page.eval_on_selector_all(
            "a",
            """elements => elements.filter(e => 
                    e.href.includes("job-opening") && !e.href.includes("#main")
                )
                .map(e => e.href)
            """
        )

    def should_visit(self, url: str) -> bool:
        keywords =  [
            "developer",
            "engineer",
            "architect",
            "developer",
            "analyst",
            "analytics",
            "technician",
            "administrator",
        ]
        
        is_tech_job = any(keyword.lower() in url.lower() for keyword in keywords)
        
        return is_tech_job
    
    
    def results_filter(self, url: str) -> bool:
        return "fromsearch" not in url and "#main" not in url