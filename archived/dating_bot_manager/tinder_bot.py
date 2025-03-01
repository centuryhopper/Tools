import os
from pathlib import Path
import random as random
import time

import undetected_chromedriver as uc
from selenium.common.exceptions import (ElementNotInteractableException,
                                        NoSuchElementException,
                                        StaleElementReferenceException)
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait


from bot_manager_abstract import DatingBot


class TinderBot(DatingBot):
    """Implementation of DatingBot for Tinder"""
    MAX_NUM_FILES = 20

    def __init__(self, username, password, unique_name, configs_path):
        """Initialize the WebDriver"""
        self.screenshots_path = os.path.join(configs_path, "screenshots")
        self.photo_file_path = os.path.join(configs_path, f'{unique_name}_pics')
        os.makedirs(self.screenshots_path, exist_ok=True)
        os.makedirs(self.photo_file_path, exist_ok=True)
        self.username = username
        self.password = password
        self.random = random.Random()
        options = uc.ChromeOptions()
        options.binary_location = '/snap/bin/chromium'
        options.add_argument('--private')
        options.add_argument('--disable-dev-shm-usage')
        options.add_argument('--no-sandbox')
        options.add_argument('--disable-gpu')
        options.add_argument("--incognito")
        options.add_argument("--disable-notifications")  # Disable notifications
        options.add_argument("--disable-popup-blocking")  # Disable pop-ups
        self.driver = uc.Chrome(headless=False, use_subprocess=False,)
        # self.driver.set_window_position(0, -1000)
        self.driver.implicitly_wait(5)
        self.driver.get("https://tinder.com")
        # print('hi from tinder bot')

    def wait_and_get_all(self, xpath, seconds=5):
        try:
            wait = WebDriverWait(self.driver, seconds)
            elements = wait.until(lambda driver: self._find_all_elements(xpath))
            return elements if elements else None
        except Exception as e:
            print(e)
            return None

    def wait_and_get(self, xpath, seconds=2.5):
        try:
            wait = WebDriverWait(self.driver, seconds)
            element = wait.until(lambda driver: self._find_and_display_element(xpath))
            return element if element else None
        except Exception as e:
            print(e)
            return None
    
    def _find_and_display_element(self, xpath):
        try:
            element = self.driver.find_element(By.XPATH, xpath)
            return element if element.is_displayed() else None
        except (StaleElementReferenceException, NoSuchElementException, Exception):
            return None

    def _find_all_elements(self, xpath):
        try:
            elements = self.driver.find_elements(By.XPATH, xpath)
            return elements if all(e.is_displayed() for e in elements) else None
        except (StaleElementReferenceException, NoSuchElementException, Exception):
            return None


    def screenshot(self, path=''):
        """Take a screenshot of the current browser window"""
        try:
            screenshot_pathname = os.path.join(path or self.screenshots_path, '{}.png'.format(time.strftime("%Y_%m_%d_%H_%M_%S")))
            print(screenshot_pathname)
            if self.driver:
                self.driver.save_screenshot(screenshot_pathname)
            if len(os.listdir(self.screenshots_path)) > TinderBot.MAX_NUM_FILES:
                for f in Path(self.screenshots_path).iterdir():
                    if f.is_file():
                        f.unlink()
        except Exception as e:
            pass
        

    def get_button_parent(self, e):
        if not e:
            return None
        parent = e.find_element(By.XPATH, '..')
        if parent.tag_name.lower() == 'button':
            return e
        # traverse up
        return self.get_button_parent(parent)
    
    def login(self):
        self.screenshot()
        buttons = self.driver.find_elements(By.TAG_NAME, "button")
        for button in buttons:
            if button and "i decline" in button.text.lower():
                button.click()
                break

        try:
            google_close_button = self.driver.find_element(By.ID, "close")
            google_close_button.click()
        except Exception:
            pass

        try:
            login_button = self.wait_and_get("//a[contains(@href,'https://tinder.onelink.me/9K8a/3d4abb81')]")
            login_button.click()

            facebook_button = self.wait_and_get("//button[contains(@aria-label,'Log in with Facebook')]")
            facebook_button.click()

            self.driver.switch_to.window(self.driver.window_handles[1])

            username_field = self.wait_and_get("//*[@id='email']")
            password_field = self.wait_and_get("//*[@id='pass']")

            username_field.send_keys(self.username)
            password_field.send_keys(self.password)
            self.screenshot()

            login_facebook_button = self.wait_and_get("//input[starts-with(@id, 'u_0_0_')]")
            login_facebook_button.click()

            continue_as_button = self.wait_and_get("//span[contains(text(), 'continue as')]")
            continue_as_button.click()
            time.sleep(15)
            self.screenshot()
        except Exception as e:
            pass
        print(self.driver.window_handles)
        self.driver.switch_to.window(self.driver.window_handles[0])
        self.handle_popups(3000)

    def try_login(self) -> bool:
        """Login to Tinder"""
        tries = 0
        could_login = not self.wait_and_get("//div[text()='Create account']")
        time.sleep(3)

        while tries < 3 and not could_login:
            tries += 1
            self.screenshot()
            buttons = self.driver.find_elements(By.TAG_NAME, "button")
            for button in buttons:
                if button and "i decline" in button.text.lower():
                    button.click()
                    break

            try:
                google_close_button = self.driver.find_element(By.ID, "close")
                google_close_button.click()
            except Exception:
                pass

            try:
                login_button = self.wait_and_get("//a[contains(@href,'https://tinder.onelink.me/9K8a/3d4abb81')]")
                login_button.click()

                facebook_button = self.wait_and_get("//button[contains(@aria-label,'Log in with Facebook')]")
                facebook_button.click()

                self.driver.switch_to.window(self.driver.window_handles[1])

                username_field = self.wait_and_get("//*[@id='email']")
                password_field = self.wait_and_get("//*[@id='pass']")

                username_field.send_keys(self.username)
                password_field.send_keys(self.password)
                self.screenshot()

                login_facebook_button = self.wait_and_get("//input[starts-with(@id, 'u_0_0_')]")
                login_facebook_button.click()

                continue_as_button = self.wait_and_get("//span[contains(text(), 'continue as')]")
                continue_as_button.click()

                self.driver.switch_to.window(self.driver.window_handles[0])

                time.sleep(self.random.uniform(10, 15))
                self.screenshot()

                could_login = not self.wait_and_get("//div[text()='Create account']")
            except Exception:
                pass

            time.sleep(self.random.uniform(5, 10))

        if not could_login:
            return False

        self.handle_popups(self.random.uniform(3, 4))
        return True

    def dispose(self) -> bool:
        """Close the WebDriver"""
        if self.driver:
            self.driver.quit()
            return True
        return False

    def handle_popups(self, milliseconds=0.):
        """Handle popups such as location access and notifications"""
        print("Handling popups...")
        time.sleep(milliseconds/1000)
        self.click_button_with_text('Allow')
        self.click_button_with_text('Not interested')
        self.click_button_with_text('miss out')
        try:
            decline_cookie_button = self.wait_and_get("//div[contains(text(), 'I decline')]")
            decline_cookie_button = self.get_button_parent(decline_cookie_button)
            decline_cookie_button.click()
        except Exception as e:
            print(e)
        try:
            self.click_button_with_text('not interested', 'no thanks')
        except Exception as e:
            print(e)
        self.click_button_with_text('maybe later')
        self.press_escape()
    
    def click_button_with_text(self,*args:str):
        self.screenshot()
        try:
            buttons = self.driver.find_elements(By.TAG_NAME, 'button')
            for button in buttons:
                if button and button.text.lower() in args:
                    button.click()
                    break
        except ElementNotInteractableException as e:
            print(e)
        except StaleElementReferenceException as e:
            print(e)
        except Exception as e:
            print(e)
    
    def send_message_to_matches(self):
        """Send messages to matches"""
        print("Sending messages to matches...")
    
    def like(self) -> bool:
        """Swipe right"""
        time.sleep(self.random.uniform(0.5, 1.5))
        try:
            self.screenshot()
            # like_span = self.wait_and_get("//span[text()='Like']")
            like_span = self.driver.find_element(By.XPATH, "//span[text()='Like']")
            like_span = self.get_button_parent(like_span)
            like_span.click()
            # ActionChains(self.driver).send_keys(Keys.ALT).send_keys(Keys.TAB).perform()
            return True
        except Exception as e:
            return False
        
    def dislike(self):
        """Swipe left"""
        try:
            dislike_span = self.wait_and_get("//span[text()='Nope']")
            dislike_span = self.get_button_parent(dislike_span)
            dislike_span.click()
        except Exception as e:
            pass
    
    def no_more_likes(self) -> bool:
        """Check if like limit has been reached"""
        out_of_likes = None
        try:
            out_of_likes = self.driver.find_element(By.XPATH, "//h3[text()='Select a plan']")
            # re-assign if null
            out_of_likes = self.driver.find_element(By.XPATH, "//h3[text()='Unlimited Likes. Send as many likes as you want.']") if not out_of_likes else out_of_likes
        except Exception as e:
            pass
        return out_of_likes is not None

    def ran_out_of_matches(self) -> bool:
        """Check if no more matches are available"""
        try:
            out_of_matches_notice = self.driver.find_element(By.XPATH, "//div[contains(text(),'We are unable to find any potential matches right now. Try changing your preferences to see who is nearby')]")
            return True
        except Exception as e:
            return False
    
    def press_escape(self):
        try:
            ActionChains(self.driver).send_keys(Keys.ESCAPE).perform()
        except Exception as e:
            pass

    def auto_swipe(self):
        """Automate swiping process"""
        time.sleep(30)
        print('running now!')
        while True:
            if not self.like():
                if self.no_more_likes():
                    break
                if self.ran_out_of_matches():
                    self.driver.refresh()
                # self.press_escape()
                # self.handle_popups()