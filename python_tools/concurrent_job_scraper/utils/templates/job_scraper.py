from abc import ABC, abstractmethod
from typing import List
from playwright.async_api import Page

class JobScraper(ABC):
    def __init__(self, start_url: str, name: str):
        self._start_url = start_url
        self._name = name

    @property
    def start_url(self) -> str:
        return self._start_url

    @property
    def name(self) -> str:
        return self._name

    @abstractmethod
    def is_job_page(self, url: str) -> bool:
        """
        Return True if this URL is an individual job posting page.
        Must be implemented by subclass.
        """
        pass

    @abstractmethod
    async def get_links(self, page: Page) -> List[str]:
        pass

    @abstractmethod
    def should_visit(self, url: str) -> bool:
        """
        Return True if the crawler should visit this URL.
        Must be implemented by subclass.
        """
        pass

    @abstractmethod
    def results_filter(self, url: str) -> bool:
        """
        Return True if this URL should be included in results.
        Must be implemented by subclass.
        """
        pass