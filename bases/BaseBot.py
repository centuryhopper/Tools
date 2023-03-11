# the line below avoids auto generating a __pycache__ directory when this script is running
import sys
sys.dont_write_bytecode = True
import os
import random
from time import sleep, strftime
import time
from selenium import webdriver
from selenium.common.exceptions import (ElementClickInterceptedException,
                                        NoSuchElementException,
                                        TimeoutException)
from selenium.webdriver.firefox.options import Options
from selenium.webdriver.firefox.service import Service
from selenium.webdriver.common.by import By
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.wait import WebDriverWait
from webdriver_manager.firefox import GeckoDriverManager
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.keys import Keys
from pathlib import Path
from abc import ABC, abstractmethod



class AbstractBot(ABC):
    def __init__(self, driver, url) -> None:
        os.chdir(os.path.dirname(__file__))
        self.driver = driver
        self.driver.get(url)
        self.driver.implicitly_wait(3)
        self.MAX_NUM_FILES = 20

    @abstractmethod
    def screenshot(self, screenshotsPath=f'{os.getcwd()}/screenshots/'):
        self.screenshotsPath = screenshotsPath
        if not os.path.exists(self.screenshotsPath):
            os.makedirs(self.screenshotsPath)
        try:
            self.driver.save_screenshot(f'{self.screenshotsPath}{strftime("%Y-%m-%d-%H_%M_%S")}.png')
            if len(os.listdir(self.screenshotsPath)) > self.MAX_NUM_FILES:
                for f in Path(self.screenshotsPath).iterdir():
                    if f.is_file():
                        f.unlink()
        except:
            pass

    @abstractmethod
    def waitAndGet(self,searchQuery:str,by:str = By.XPATH,seconds=random.uniform(1,2), onDriver=None) -> WebDriverWait:
        if not onDriver:
            onDriver = self.driver
        try:
            e = WebDriverWait(onDriver, seconds).until(
            EC.presence_of_element_located(
                (by, searchQuery))
            )
            return e
        except Exception as e:
            return None

    @abstractmethod
    def waitAndGetAll(self,searchQuery:str,by:str = By.XPATH,seconds=random.uniform(1,2),onDriver=None) -> WebDriverWait:
        if not onDriver:
            onDriver = self.driver
        try:
            e = WebDriverWait(onDriver, seconds).until(
            EC.presence_of_all_elements_located(
                (by, searchQuery))
            )
            return e
        except Exception as e:
            return None




