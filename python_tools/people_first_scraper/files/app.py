import os
import pprint
import random
import re
import sys
import time
import datetime
from pathlib import Path
import subprocess

import pandas as pd
from selenium import webdriver
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.by import By
from selenium.webdriver.common.proxy import Proxy, ProxyType
from selenium.webdriver.firefox.options import Options
from selenium.webdriver.firefox.service import Service

os.chdir(os.path.dirname(__file__))


def human_delay(seconds=random.uniform(2, 5)):
    time.sleep(seconds)


def setup_firefox_driver():
    options = Options()
    options.add_argument("--headless")

    # Random User-Agent
    user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36"
    options.set_preference("general.useragent.override", user_agent)

    # Disable WebDriver detection
    options.set_preference("dom.webdriver.enabled", False)
    options.set_preference("useAutomationExtension", False)

    service = Service(executable_path="./geckodriver")

    driver = webdriver.Firefox(
        options=options,
    )

    # Hide WebDriver attribute
    driver.execute_script(
        "Object.defineProperty(navigator, 'webdriver', {get: () => undefined})"
    )

    return driver


def didRanAlready():
    script_execution_records = Path.cwd() / "script_execution_records"
    script_execution_records.mkdir(exist_ok=True)
    if not os.path.exists(
        f"{os.getcwd()}/script_execution_records/people_first_time_stamp.txt"
    ):
        # create the file if it doesn't exist
        open(
            f"{os.getcwd()}/script_execution_records/people_first_time_stamp.txt", "a"
        ).close()

    with open(
        f"{os.getcwd()}/script_execution_records/people_first_time_stamp.txt", "r"
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
        f"{os.getcwd()}/script_execution_records/people_first_time_stamp.txt", "w"
    ) as f:
        f.write(time.strftime("%Y-%m-%d-%H:%M:%S"))
    return False


def scrape_salary_from_job_page(driver: webdriver.Firefox, job_url: str):
    driver.get(job_url)
    human_delay(random.uniform(3, 6))

    # case insensitive search
    salary_element = None
    try:
        p_tags = driver.find_elements(By.TAG_NAME, "p")
        for p_tag in p_tags:
            if "salary:" in p_tag.text.lower():
                salary_element = p_tag
                break
            if (
                "this position is no longer being advertised in people first."
                in p_tag.text.lower()
            ):
                return "job unavailable"

        if not salary_element:
            raise Exception()
    except:
        return "n/a"

    return re.sub("Salary:", "", salary_element.text, flags=re.IGNORECASE).strip()


def scrape_table_data(driver):
    KEYWORDS = [
        "analyst",
        "programmer",
        "specialist",
        "data",
        "computer",
    ]
    # go thru each link and look for those that include one of the keywords
    human_delay()
    search_results_shell = driver.find_element(By.CLASS_NAME, "searchResultsShell")
    # print(search_results_shell.text)
    table = search_results_shell.find_element(By.TAG_NAME, "table")
    # print(table.text)
    tbody = table.find_element(By.TAG_NAME, "tbody")
    rows = tbody.find_elements(By.TAG_NAME, "tr")
    result_dict = {
        "Title": [],
        "Salary": [],
        "Date Posted": [],
        "Url": [],
    }
    # store the title, salary, date posted, and url to a pandas dataframe
    for row in rows:
        td_cells = row.find_elements(By.TAG_NAME, "td")
        for cell in td_cells:
            # print(cell.get_attribute('headers'))
            match cell.get_attribute("headers"):
                case "hdrTitle":
                    # print(cell.text)
                    a = cell.find_element(By.TAG_NAME, "a")
                    href = a.get_attribute("href")
                    if not a or not any(
                        keyword in a.text.lower() for keyword in KEYWORDS
                    ):
                        break
                    result_dict["Title"].append(a.text)
                    result_dict["Url"].append(href)
                case "hdrDate":
                    span = cell.find_element(By.TAG_NAME, "span")
                    result_dict["Date Posted"].append(span.text)

    for job_url in result_dict["Url"]:
        result_dict["Salary"].append(scrape_salary_from_job_page(driver, job_url))
    # pprint.pprint(result_dict)

    return result_dict


# by default, remove all files from directory if accumulated for more than 30 days
def clean_up_people_job_results(PATH, MAX_NUM_FILES=30):
    if len(os.listdir(PATH)) > MAX_NUM_FILES:
        for f in Path(PATH).iterdir():
            if f.is_file():
                f.unlink()


def main():
    if didRanAlready():
        return
    driver = setup_firefox_driver()
    LINK = "https://jobs.myflorida.com/go/Science%2C-Technology%2C-Engineering-&-Mathematics/2814200/"
    driver.get(LINK)

    list_of_dfs = []

    try:
        pagination_bottom = driver.find_element(By.CLASS_NAME, "pagination-bottom")
        # grab other pages if any
        ul = pagination_bottom.find_element(By.CLASS_NAME, "pagination")
        ul = ul.find_elements(By.TAG_NAME, "li")
        i = 0
        PAGE_LINK = "https://jobs.myflorida.com/go/Science%2C-Technology%2C-Engineering-&-Mathematics/2814200/$$$/?q=&sortColumn=referencedate&sortDirection=desc"
        pages = []
        for li in ul:
            a_title = li.find_element(By.TAG_NAME, "a").get_attribute("title")
            if a_title == "First Page" or a_title == "Last Page":
                continue
            pages.append(PAGE_LINK.replace("$$$", str(i)))
            i += 25
        pprint.pprint(pages)
        for page in pages:
            driver.get(page)
            human_delay()
            df = pd.DataFrame(scrape_table_data(driver))
            df = df[df["Salary"] != "job unavailable"]
            list_of_dfs.append(df)

        result = pd.concat(list_of_dfs, ignore_index=True)
        # pprint.pprint(result)

        if os.path.exists("./PeopleFirstCareersOutput/"):
            os.mkdir("./PeopleFirstCareersOutput/")

        clean_up_people_job_results(PATH="./PeopleFirstCareersOutput/")

        result.to_csv(
            f'./PeopleFirstCareersOutput/results_{time.strftime("%Y_%m_%d_%H_%M_%S")}.csv'
        )

        # provide an accurate finish time
        with open(
            f"{os.getcwd()}/script_execution_records/people_first_time_stamp.txt", "w"
        ) as f:
            f.write(time.strftime("%Y-%m-%d-%H:%M:%S"))
    finally:
        driver.quit()


if __name__ == "__main__":
    main()
