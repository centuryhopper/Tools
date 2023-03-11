import random
from selenium.webdriver.common.by import By
from selenium.webdriver.support.wait import WebDriverWait
from BaseBot import AbstractBot

class ActionBot(AbstractBot):
    def __init__(self, driver, url, username, password, name) -> None:
        super().__init__(driver, url)
        self.username = username
        self.password = password
        self.name = name

    def screenshot(self, screenshotsPath=...):
        super().screenshot(screenshotsPath)

    def waitAndGet(self,searchQuery:str,by:str = By.XPATH,seconds=random.uniform(1,2), onDriver=None) -> WebDriverWait:
        return super().waitAndGet(searchQuery,by,seconds,onDriver)

    def waitAndGetAll(self,searchQuery:str,by:str = By.XPATH,seconds=random.uniform(1,2),onDriver=None) -> WebDriverWait:
        return super().waitAndGetAll(searchQuery,by,seconds,onDriver)


