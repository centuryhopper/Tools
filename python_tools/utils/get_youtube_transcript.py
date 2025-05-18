#!/home/leo_zhang/miniconda3/envs/web_auto/bin/python


# -*- coding: utf-8 -*-
"""
Created on 5/21/2025
@author: Leo Zhang

Title: get_youtube_transcript.py

Description: get transcript (auto-generated) from any youtube video.

"""

from typing import List, Optional
import pandas as pd
from time import sleep, strftime
import os
from selenium import webdriver
from selenium.webdriver.remote.webelement import WebElement
from selenium.common.exceptions import (ElementClickInterceptedException,
ElementNotInteractableException,
                                        NoSuchElementException,
                                        TimeoutException)
from selenium.webdriver.firefox.options import Options
from selenium.webdriver.firefox.service import Service
from selenium.webdriver.common.by import By
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait
from webdriver_manager.firefox import GeckoDriverManager
from selenium.webdriver.common.desired_capabilities import DesiredCapabilities
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains


os.chdir(os.path.dirname(__file__))
LOG_PATH = './logs'
os.makedirs(LOG_PATH, exist_ok=True)
PATH_TO_TRANSCRIPT = './yt_transcripts'
os.makedirs(PATH_TO_TRANSCRIPT, exist_ok=True)


def open_url_in_chrome(url, mode="headed"):
    firefox_options = webdriver.FirefoxOptions()
    # firefox__options.add_argument('--no-sandbox')
    firefox_options.add_argument('--disable-blink-features=AutomationControlled')
    if mode == 'headless':
        firefox_options.add_argument("--headless")
    firefox_options.add_argument("--incognito")
    firefox_options.add_argument('--disable-dev-shm-usage')
    firefox_options.add_argument('--ignore-ssl-errors=yes')
    firefox_options.add_argument('--ignore-certificate-errors')
    firefox_options.add_argument("--disable-infobars")
    firefox_options.add_argument("--disable-extensions")
    firefox_options.add_argument("--disable-popup-blocking")
    firefox_options.add_argument("--disable-notifications")
    firefox_options.add_argument("--disable-web-security")

    firefox_options.add_argument("--private")
    # firefox_options.add_argument("--disable-setuid-sandbox")
    firefox_options.add_argument("----safebrowsing-disable-download-protection")
    # firefox_options.add_argument("--disable-crash-reporter")
    # firefox_options.add_argument("--disable-background-timer-throttling")

    firefox_options.set_preference('geo.prompt.testing', True)
    firefox_options.set_preference('geo.prompt.testing.allow', True)
    # This will mock a certain location:
    firefox_options.set_preference('geo.provider.network.url',
    'data:application/json,{"location": {"lat": 28.538336, "lng": -81.379234}, "accuracy": 100.0}')
    d = webdriver.Firefox(service=Service(GeckoDriverManager().install(), log_path=os.path.join(LOG_PATH, 'yt_transcript.log')), options=firefox_options,)
    d.get(url)
    return d

def accept_T_and_C(driver):
    # Click 'No thanks'
    driver.find_element(By.XPATH,"//paper-button[@aria-label='No thanks']").click()

    # Click 'I agree' https://stackoverflow.com/questions/64846902/how-to-get-rid-of-the-google-cookie-pop-up-with-my-selenium-automation
    driver.switch_to.frame(
        driver.find_element(By.XPATH,"//iframe[contains(@src, 'consent.google.com')]")
    )
    sleep(1)
    driver.find_element(By.XPATH,'//*[@id="introAgreeButton"]/span/span').click()
    sleep(3)
    driver.refresh()

def get_transcript(driver: webdriver.Firefox, mode):
    driver.implicitly_wait(10)

    # try:
    #     print("Accepting Terms and Conditions")
    #     accept_T_and_C(driver)
    # except:
    #     print("No T&Cs to accept.")
    driver.refresh()
    sleep(3)
        
    try:
        # Click 'No thanks'
        show_transcript = driver.find_element(By.XPATH, "//button[@aria-label='No thanks']")
        show_transcript.click()
    except Exception as e:
        pass

    print("Opening transcript")
    try:
        # Click 'More actions'
        button = driver.find_element(By.ID, "expand")
        button.click()
    except Exception as e:
        pass
    
    try:
        # Click 'Show transcript'
        show_transcript = driver.find_element(By.XPATH, "//button[@aria-label='Show transcript']")
        show_transcript.click()
    except Exception as e:
        pass
    sleep(3)

    # Get all transcript text
    print("Copying transcript ")
    transcript = ''
    try:
        transcript_element = driver.find_element(By.ID,
        "panels")
        transcript = transcript_element.text
    except Exception as e:
        pass

    return transcript

def transcript2df(transcript):
    transcript = transcript.split("\n")
    transcript_timestamps = transcript[1::2]
    transcript_text = transcript[::2]
    # make sure arrays are the same length
    min_len = min(len(transcript_timestamps), len(transcript_text))
    df = pd.DataFrame({"timestamp": transcript_timestamps[:min_len], "text": transcript_text[:min_len]})

    return df

def main(url, mode="headless"):
    driver : Optional[webdriver.Firefox] = None
    try:
        driver = open_url_in_chrome(url, mode)
        transcript = get_transcript(driver, mode)

        df = transcript2df(transcript)

        print("Saving transcript ")
        
        df.to_csv(os.path.join(PATH_TO_TRANSCRIPT, f'my_transcript_timestamped_{strftime("%Y-%m-%d-%H_%M_%S")}.csv'), index=False)
        with open(os.path.join(PATH_TO_TRANSCRIPT, f'my_transcript_text_only_{strftime("%Y-%m-%d-%H_%M_%S")}.txt'), "w") as text_file:
            print(" ".join(" ".join(df.text.values).split()), file=text_file)
        print(f"Transcript saved to: {PATH_TO_TRANSCRIPT}")
    except Exception as e:
        raise e
    finally:
        driver.quit()
    


if __name__ == "__main__":
    # url = "https://www.youtube.com/watch?v=Fa_V9fP2tpU&t=701s"
    
    while 'https://www.youtube.com/watch?v=' not in (url := input('Enter a youtube video url: ')):
        print('please try again')
    
    while (mode := input("select 1 for headed mode or 2 for headless mode: ")) not in list('12'):
        print('please try again')
        
    match int(mode):
        case 1:
            main(url, 'headed')
        case 2:
            main(url, 'headless')
