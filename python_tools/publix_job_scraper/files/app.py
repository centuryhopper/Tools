#!/home/leo_zhang/miniconda3/envs/web_auto/bin/python


# -*- coding: utf-8 -*-
"""
Created on 5/6/2025
@author: Leo Zhang

Description: Check for associate and intermediate software engineer roles at publix

"""

import datetime
import os
import random
import re
from re import Pattern
import subprocess
import time
from pathlib import Path
from typing import List

import pandas as pd
from selenium import webdriver
from webdriver_manager.firefox import GeckoDriverManager
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.by import By
from selenium.webdriver.common.proxy import Proxy, ProxyType
from selenium.webdriver.firefox.options import Options
from selenium.webdriver.firefox.service import Service
from selenium.webdriver.remote.webelement import WebElement

class scrapped_data:
    def __init__(self, title:str, salary_range:str, date_posted:str, url:str, location:str, job_id:str):
        self.title = title
        self.salary_range = salary_range
        self.url = url
        self.location = location
        self.date_posted = date_posted
        self.job_id = job_id

class regex_patterns:
    # Regex pattern: starts with $, ends with k (e.g., "$20k", "$7.5k")
    # \S is any non-whitespace character
    SALARY_PATTERN :Pattern[str] = re.compile(r"^\$\d{2,3}\s-\s\d{3}k$", re.IGNORECASE)
    LOCATION_PATTERN :Pattern[str] = re.compile(r'Location:(.+)', re.IGNORECASE)
    DATE_POSTED_PATTERN :Pattern[str] = re.compile(r'Date Posted:(.+)', re.IGNORECASE)
    JOB_ID_PATTERN :Pattern[str] = re.compile(r'Job ID:(.+)', re.IGNORECASE)
    

os.chdir(os.path.dirname(__file__))
LOG_PATH = './logs'
output = Path.cwd() / "PublixCorporateCareersOutput"
output.mkdir(exist_ok=True)

def human_delay(seconds=random.uniform(2, 5)):
    time.sleep(seconds)

def setup_firefox_driver():
    firefox_options = Options()
    firefox_options.binary_location = "/usr/bin/firefox"
    # options.add_argument("--headless")

    # Random User-Agent
    user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36"
    firefox_options.set_preference("general.useragent.override", user_agent)

    # Disable WebDriver detection
    firefox_options.set_preference("dom.webdriver.enabled", False)
    firefox_options.set_preference("useAutomationExtension", False)

    # print(os.getcwd())
    # print(os.listdir())

    # service = Service(executable_path="./geckodriver_for_arm")
    service = Service(GeckoDriverManager().install(), log_path=os.path.join(LOG_PATH, 'publix_careers.log'))
    driver = webdriver.Firefox(service=service, options=firefox_options)

    # Hide WebDriver attribute
    driver.execute_script(
        "Object.defineProperty(navigator, 'webdriver', {get: () => undefined})"
    )

    return driver

def didRanAlready():
    script_execution_records = Path.cwd() / "script_execution_records"
    script_execution_records.mkdir(exist_ok=True)
    if not os.path.exists(
        f"{os.getcwd()}/script_execution_records/publix_corporate_time_stamp.txt"
    ):
        # create the file if it doesn't exist
        open(
            f"{os.getcwd()}/script_execution_records/publix_corporate_time_stamp.txt",
            "a",
        ).close()

    with open(
        f"{os.getcwd()}/script_execution_records/publix_corporate_time_stamp.txt", "r"
    ) as f:
        lastReadDateTime = f.read().strip()

        # region handle file creation case
        if not lastReadDateTime:
            # epoch start time so that we're guaranteed to run the program when the file is created
            # since it has definitely been at 24 hours since 1970
            lastReadDateTime = "1970-01-01-00:00:00"
            # with open(f'{os.getcwd()}/script_execution_records/{name}.txt', 'w') as dummy:
            #     dummy.write(lastReadDateTime)
        # endregion

        lastReadDateTime = datetime.datetime.strptime(
            lastReadDateTime, "%Y-%m-%d-%H:%M:%S"
        )
        # print(lastReadDateTime)
        diff = datetime.datetime.now() - lastReadDateTime
        diff_in_hours = diff.total_seconds() / 3600
        # print(name, diff_in_hours)
        wifiCheck = subprocess.run(
            ["curl", "-Is", "https://www.google.com"], capture_output=True, text=True
        ).stdout
        if diff_in_hours < 24 or not "HTTP/2 200" in wifiCheck:
            return True
    # avoid a cron job trigger while the first trigger is still running
    with open(
        f"{os.getcwd()}/script_execution_records/publix_corporate_time_stamp.txt", "w"
    ) as f:
        f.write(time.strftime("%Y-%m-%d-%H:%M:%S"))
    return False

def scrape_info_from_job_page(driver: webdriver.Firefox, job_url: str) -> scrapped_data | None:
    driver.get(job_url)
    human_delay(random.uniform(3, 6))
    h1 = driver.find_element(By.TAG_NAME, "h1")
    salary_range = ''
    # Get only divs (or narrow this further with a class or ID if you can)
    divs = driver.find_elements(By.TAG_NAME, "div")

    # Loop through and match
    for div in divs:
        text = div.text.strip()
        if regex_patterns.SALARY_PATTERN.match(text):
            # print("Matched:", text)
            salary_range = text
            break

    # Locate the <strong> element by its text
    strong_elem = driver.find_element(By.XPATH, "//strong[contains(text(), 'Location')]")
    
    heading_p_tag = strong_elem.find_element(By.XPATH, "..")
    location_results = regex_patterns.LOCATION_PATTERN.search(heading_p_tag.text)
    job_id_results = regex_patterns.JOB_ID_PATTERN.search(heading_p_tag.text)
    date_posted_results = regex_patterns.DATE_POSTED_PATTERN.search(heading_p_tag.text)
    
    if not location_results or not job_id_results or not date_posted_results:
        return None
    
    return scrapped_data(
        title=h1.text,
        url=job_url,
        salary_range=salary_range,
        location=location_results.group(1).strip(),
        job_id=job_id_results.group(1).strip(),
        date_posted=date_posted_results.group(1).strip(),
    )

def scrape_table_data(driver):
    KEYWORDS = [
        "intermediate",
        "software",
        "analyst",
        "associate",
        "developer",
        "full stack",
    ]
    # go thru each link and look for those that include one of the keywords
    human_delay()
    publix_job_cards_div = driver.find_element(By.XPATH, '//div[contains(@class, "wp-block-publix-cards")]')
    
    # Get the urls for each card such that its job title contains text that match at least one the terms in the KEYWORDS list
    # base_url = 'https://jobs.publix.com'
    url_lst: List[str] = []
    a_tags: List[WebElement] = publix_job_cards_div.find_elements(By.TAG_NAME, "a")
    for a_tag in a_tags:
        job_title = a_tag.get_attribute('data-gtm-jn')
        # make sure methods match
        if not job_title or not any(
                keyword in job_title.lower() for keyword in KEYWORDS
            ):
            continue
        url_lst.append(f"{a_tag.get_attribute('href')}")
    result_dict = {
        "Title": [],
        "Salary": [],
        "Date Posted": [],
        "Url": [],
        "Location": [],
        "Job ID": [],
    }
    
    # store the title, salary, date posted, and url to a pandas dataframe
    for url in url_lst:
        data: scrapped_data | None = scrape_info_from_job_page(driver, url)
        if not data: continue
        result_dict['Title'].append(data.title)
        result_dict['Salary'].append(data.salary_range)
        result_dict['Date Posted'].append(data.date_posted)
        result_dict['Url'].append(data.url)
        result_dict['Location'].append(data.location)
        result_dict['Job ID'].append(data.job_id)
        
    # pprint.pprint(result_dict)

    return result_dict

# by default, remove all files from directory if accumulated for more than 30 days
def clean_up_publix_job_results(PATH, MAX_NUM_FILES=30):
    if len(os.listdir(PATH)) > MAX_NUM_FILES:
        for f in Path(PATH).iterdir():
            if f.is_file():
                f.unlink()

def main():
    if didRanAlready():
        return
    driver: webdriver.Firefox = setup_firefox_driver()
    driver.get('https://jobs.publix.com/jobs/?department_type_slug=publix-technology')
    list_of_dfs = []
    try:
        get_page_link = lambda x: f"https://jobs.publix.com/jobs/?department_type_slug=publix-technology&p={x}"
        nav: WebElement = driver.find_element(By.CSS_SELECTOR, 'nav[aria-label="Pagination navigation"]')
        ul = nav.find_element(By.TAG_NAME, "ul")
        ul_children = ul.find_elements(By.XPATH, "./*")
        # exclude the first and last li element
        n = len(ul_children) - 2
        for page_number in range(1, n+1):
            driver.get(get_page_link(page_number))
            human_delay()
            df = pd.DataFrame(scrape_table_data(driver))
            # df = df[df["Salary"] != "job unavailable"]
            list_of_dfs.append(df)

        result = pd.concat(list_of_dfs, ignore_index=True)
        # pprint.pprint(result)

        clean_up_publix_job_results(PATH="./PublixCorporateCareersOutput/")

        result.to_csv(
            f"./PublixCorporateCareersOutput/results_{time.strftime('%Y_%m_%d_%H_%M_%S')}.csv"
        )

        # provide an accurate finish time
        with open(
            f"{os.getcwd()}/script_execution_records/publix_corporate_time_stamp.txt",
            "w",
        ) as f:
            f.write(time.strftime("%Y-%m-%d-%H:%M:%S"))
    except Exception as e:
        print(e)
    finally:
        driver.quit()


if __name__ == "__main__":
    main()
