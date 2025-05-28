import random
from typing import List, Set
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.chrome.options import Options
import time
from itertools import islice, takewhile
from fake_useragent import UserAgent
ua = UserAgent(os='Linux', browsers=['Chrome'])

USERNAME = input("enter your username: ")
PASSWORD = input("enter your password: ")

def create_undetectable_driver():
    chrome_options = Options()
    
    # Stealth options
    chrome_options.add_argument("--disable-blink-features=AutomationControlled")
    chrome_options.add_argument("--disable-infobars")
    chrome_options.add_argument("--disable-dev-shm-usage")
    chrome_options.add_argument("--no-sandbox")
    chrome_options.add_argument("--start-maximized")

    # Optional: run in headless mode (less detectable with --new-headless)
    # chrome_options.add_argument("--headless=new")  # safer than old headless

    # User agent spoofing
    random_ua = ua.random
    chrome_options.add_argument("user-agent={}".format(random_ua))
    print(random_ua)

    driver = webdriver.Chrome(options=chrome_options)

    # Remove navigator.webdriver flag
    driver.execute_cdp_cmd("Page.addScriptToEvaluateOnNewDocument", {
        "source": """
            Object.defineProperty(navigator, 'webdriver', {
                get: () => undefined
            });
        """
    })

    return driver

def login(driver):
    driver.get("https://www.instagram.com/accounts/login/")
    time.sleep(random.uniform(3,5))

    driver.find_element(By.NAME, "username").send_keys(USERNAME)
    driver.find_element(By.NAME, "password").send_keys(PASSWORD + Keys.RETURN)
    # give a minute for mfa auth
    # time.sleep(60)
    time.sleep(random.uniform(1,3))

def get_following_set(driver: webdriver.Chrome) -> Set[str]:
    driver.get(f"https://www.instagram.com/{USERNAME}/")
    time.sleep(1)
    
    ActionChains(driver).send_keys(Keys.ESCAPE).perform()
    time.sleep(2)

    button = driver.find_element(By.PARTIAL_LINK_TEXT, "following")
    button.click()
    time.sleep(2)
    
    popup = driver.find_element(By.XPATH, "//div[@role='dialog']")
    # get every ig username until you hit 'suggested for you'
    names = set(takewhile(lambda x: x.lower() != "suggested for you", islice(popup.text.split('\n'), 2, None, 3)))
    ActionChains(driver).send_keys(Keys.ESCAPE).perform()
    return names

def get_followers_set(driver: webdriver.Chrome, following_set: Set[str]) -> Set[str]:
    res = set()
    # go to each users page
    for following_user in following_set:
        driver.get(f"https://www.instagram.com/{following_user}/")
        button = driver.find_element(By.PARTIAL_LINK_TEXT, "following")
        button.click()
        time.sleep(2)
        popup = driver.find_element(By.XPATH, "//div[@role='dialog']")
        if USERNAME in popup.text:
            res.add(following_user)
        ActionChains(driver).send_keys(Keys.ESCAPE).perform()
    return res

def unfollow_user(driver: webdriver.Chrome, user:str):
    driver.get(f"https://www.instagram.com/{user}/")
    time.sleep(2)

    try:
        btns = driver.find_elements(By.TAG_NAME, 'button')
        for btn in btns:
            if btn and 'following' in btn.text.lower():
                btn.click()

        time.sleep(1)
        popup = driver.find_element(By.XPATH, "//div[@role='dialog']")
        popup_btns = popup.find_elements(By.XPATH, "//div[@role='button']")
        for btn in popup_btns:
            if btn and 'unfollow' in btn.text.lower():
                btn.click()
        
        print(f"Unfollowed {user}")
        time.sleep(2)
    except:
        print(f"Could not unfollow {user}")

if __name__ == "__main__":
    driver = create_undetectable_driver()
    try:
        login(driver)
        time.sleep(3)

        # get those you are following
        following_set = get_following_set(driver)
        
        # from your followers, get those who follow you back
        followers_set = get_followers_set(driver, following_set)

        not_following_back_set = following_set - followers_set
        print(f"Users not following back: {not_following_back_set}")

        for user in not_following_back_set:
            unfollow_user(driver, user)
    finally:
        driver.quit()
