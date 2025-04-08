from typing import List
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.chrome.options import Options
import time
from itertools import islice, takewhile

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
    chrome_options.add_argument("user-agent=Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
                                "AppleWebKit/537.36 (KHTML, like Gecko) "
                                "Chrome/122.0.0.0 Safari/537.36")

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
    time.sleep(3)

    driver.find_element(By.NAME, "username").send_keys(USERNAME)
    driver.find_element(By.NAME, "password").send_keys(PASSWORD + Keys.RETURN)
    # give a minute for mfa auth
    # time.sleep(60)
    time.sleep(3)


def get_follow_list(driver: webdriver.Chrome, list_type:str) -> List[str]:
    driver.get(f"https://www.instagram.com/{USERNAME}/")
    time.sleep(2)

    button = driver.find_element(By.PARTIAL_LINK_TEXT, list_type)
    button.click()
    time.sleep(2)
    
    popup = driver.find_element(By.XPATH, "//div[@role='dialog']")
    
    names = list(takewhile(lambda x: x.lower() != "suggested for you", islice(popup.text.split('\n'), 2, None, 3)))
    ActionChains(driver).send_keys(Keys.ESCAPE).perform()
    return names


def unfollow_user(driver, user):
    driver.get(f"https://www.instagram.com/{user}/")
    time.sleep(2)

    try:
        unfollow_button = driver.find_element(By.XPATH, "//button[text()='Following']")
        unfollow_button.click()
        time.sleep(1)
        confirm = driver.find_element(By.XPATH, "//button[text()='Unfollow']")
        confirm.click()
        print(f"Unfollowed {user}")
        time.sleep(2)
    except:
        print(f"Could not unfollow {user}")


if __name__ == "__main__":
    driver = create_undetectable_driver()
    try:
        login(driver)
        time.sleep(3)

        following = get_follow_list(driver, "following")
        followers = get_follow_list(driver, "followers")

        not_following_back = following - followers
        print(f"Users not following back: {not_following_back}")

        for user in not_following_back:
            unfollow_user(driver, user)
    finally:
        driver.quit()
