import platform

if platform.system() == "Linux":
    import linux_tools as tools
elif platform.system() == "Windows":
    import windows_tools as tools

import json
import os
import random
from contextlib import contextmanager
from time import sleep, perf_counter

from dotenv import load_dotenv
from fake_useragent import FakeUserAgentError, UserAgent
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.firefox.options import Options
from selenium.webdriver.firefox.service import Service
from webdriver_manager.firefox import GeckoDriverManager

load_dotenv()
os.chdir(os.path.dirname(__file__))

ua = None
while True:
    try:
        ua = UserAgent()
        break
    except FakeUserAgentError:
        print('fake user agent error')
        continue
    except Exception:
        continue

fake_agent = ua.random

@contextmanager
def driver(*args, **kwargs):
    firefox_options = Options()
    # firefox_options.add_argument('--no-sandbox')
    # firefox_options.add_argument('--start-maximized')
    # firefox_options.add_argument('--start-fullscreen')
    # firefox_options.add_argument('--single-process')
    # firefox_options.add_argument('--disable-dev-shm-usage')
    # firefox_options.add_argument("--incognito")
    # firefox_options.add_argument('--disable-blink-features=AutomationControlled')
    # firefox_options.add_argument("--headless")
    # firefox_options.add_argument(f'user-agent={fake_agent}')
    # firefox_options.add_argument('--ignore-ssl-errors=yes')
    # firefox_options.add_argument('--ignore-certificate-errors')
    # firefox_options.add_argument("--disable-infobars")
    # firefox_options.add_argument("--disable-extensions")
    # firefox_options.add_argument("--disable-popup-blocking")

    d = webdriver.Firefox(service=Service(GeckoDriverManager().install(), log_path=f'{tools.LOG_PATH}geckodriver.log'), options=firefox_options,)

    try:
        yield d
    finally:
        d.quit()

class FaceBookBot:
    def __init__(self, driver, config) -> None:
        self.driver = driver
        self.title = config['title']
        self.price = config['price']
        self.category = config['category']
        self.condition = config['condition']
        self.description = config['description']
        self.photoFilePath = [os.getcwd() + config['photoFilePath'] + fname for fname in os.listdir(os.getcwd() + config['photoFilePath'])]
        self.videoFilePath = [os.getcwd() + config['videoFilePath'] + fname for fname in os.listdir(os.getcwd() + config['videoFilePath'])]
        # print(self.photoFilePath)
        # print(self.videoFilePath)

    def listItem(self):
        driver = self.driver
        driver.get('https://www.facebook.com/marketplace/create')
        driver.maximize_window()
        driver.implicitly_wait(3)
        sleep(random.uniform(2,3))

        driver.find_element(By.ID,'email').send_keys(os.getenv('fbUser'))
        driver.find_element(By.ID,'pass').send_keys(os.getenv('fbPassword'))

        driver.find_element(By.ID, 'loginbutton').click()

        sleep(random.uniform(1,2))

        driver.get('https://www.facebook.com/marketplace/create/item/')

        sleep(random.uniform(1,2))

        targetKeywords = {
            "Title",
            "Price",
            "Category",
            "Condition",
            "Description"
        }

        lst = driver.find_elements(By.TAG_NAME, 'label')

        lst = [e for e in lst if e and e.get_attribute('aria-label') in targetKeywords]

        print(len(lst))

        title = price = category = condition = description = None

        for e in lst:
            attr = e.get_attribute('aria-label')
            if attr == 'Title':
                title = e.find_element(By.ID, e.get_attribute('for'))
            if attr == 'Price':
                price = e.find_element(By.ID, e.get_attribute('for'))
            if attr == 'Category':
                category = e.find_element(By.ID, e.get_attribute('for'))
            if attr == 'Condition':
                condition = e.find_element(By.ID, e.get_attribute('for'))
            if attr == 'Description':
                description = e.find_element(By.ID, e.get_attribute('for'))
        print(title.tag_name)
        print(price.tag_name)
        print(category.tag_name)
        print(condition.tag_name)
        print(description.tag_name)

        # sleep(60)

        # title
        title.send_keys(self.title)

        # price
        price.send_keys(self.price)

        # category
        category.send_keys(self.category)

        driver.find_element(By.CSS_SELECTOR, 'body').send_keys(Keys.DOWN)
        sleep(random.uniform(0.5,1))

        driver.find_element(By.CSS_SELECTOR, 'body').send_keys(Keys.ENTER)

        # condition
        condition.click()
        sleep(random.uniform(0.5,1))

        driver.find_element(By.CSS_SELECTOR, 'body').send_keys(Keys.DOWN)
        sleep(random.uniform(0.5,1))

        driver.find_element(By.CSS_SELECTOR, 'body').send_keys(Keys.ENTER)

        # description
        description.send_keys(self.description)

        photoInputElem = driver.find_element(By.XPATH, '//input[contains(@accept, "image/*,image/heif,image/heic")]')

        for path in self.photoFilePath:
            photoInputElem.send_keys(path)
            sleep(random.uniform(0.5,1))

        sleep(random.uniform(1,2))

        videoInputElem = driver.find_element(By.XPATH, '//input[contains(@accept, "video/*")]')

        videoInputElem.send_keys(self.videoFilePath[0])

        sleep(random.uniform(30,60))

        driver.find_element(By.XPATH, '//div[contains(@aria-label, "Next")]').click()

        sleep(random.uniform(2, 3))

        driver.find_element(By.XPATH, '//div[contains(@aria-label, "Next")]').click()

        sleep(random.uniform(2, 3))

        driver.find_element(By.XPATH, '//div[contains(@aria-label, "Publish")]').click()

        sleep(random.uniform(3,5))

        driver.save_screenshot('test.png')
        # sleep(60)
        # if os.path.exists(f'{os.getcwd()}/test.png'):
        #     os.remove('test.png')

class CraigslistBot:
    def __init__(self, driver, config=None) -> None:
        self.driver = driver
        self.url = 'https://orlando.craigslist.org/'
        self.imgs = [os.getcwd() + config['photoFilePath'] + fname for fname in os.listdir(os.getcwd() + "/product_imgs/skateboard/")]
        # print(config)
        self.config = config
        driver.get(self.url)
        driver.implicitly_wait(3)
        self.driver.find_element(By.ID, "post").click()

    def listItem(self):
        driver = self.driver

        def getRadioButtonByText(text, className):
            # print(f'text to find: {text}')

            radioButtons = driver.find_elements(By.XPATH, "//input[@type='radio']")
            texts = driver.find_elements(By.CLASS_NAME, className)

            radioButtons = [rb for rb in radioButtons if rb]
            texts = [t.text.strip() for t in texts if t and t.text]

            # print(len(radioButtons), len(texts))

            for t,rb in zip(texts,radioButtons):
                if rb:
                    if text in t:
                        # print(f"FOUND radio button: {t}")
                        rb.click()
                        break

        # driver.maximize_window()

        sleep(random.uniform(2,3))

        getRadioButtonByText(self.config['posting-type'], 'right-side')

        try:
            driver.find_element(By.XPATH, '//button[@value="Continue"]').click()
        except Exception as e:
            pass

        sleep(random.uniform(2,3))

        getRadioButtonByText(self.config['category'], 'option-label')

        try:
            driver.find_element(By.XPATH, '//button[@value="continue"]').click()
        except Exception as e:
            pass

        postingTitle = self.config['title']
        postalCode = self.config['zip-code']
        description = self.config['description']
        email = self.config['my-email']
        price = self.config['price']

        driver.find_element(By.ID,'PostingTitle').send_keys(postingTitle)
        driver.find_element(By.NAME,'price').send_keys(price)
        driver.find_element(By.ID,'postal_code').send_keys(postalCode)
        driver.find_element(By.ID,'PostingBody').send_keys(description)
        driver.find_element(By.NAME,'FromEMail').send_keys(email)

        sleep(random.uniform(2,3))

        try:
            driver.find_element(By.XPATH, '//button[@value="continue"]').click()
        except Exception as e:
            pass

        sleep(random.uniform(2,3))

        try:
            # move past the gps part
            for button in driver.find_elements(By.TAG_NAME, 'button'):
                if 'continue bigbutton' in button.get_attribute('class'):
                    button.click()
                    break
            print('moved past gps part')
        except Exception as e:
            pass

        try:
            # upload images here
            print('uploading images')
            for img in self.imgs:
                driver.find_element(By.XPATH,'//input[@type="file"]').send_keys(img)
        except Exception as e:
            pass

        sleep(random.uniform(2,3))

        try:
            driver.find_element(By.XPATH, '//button[@value="Done with Images"]').click()
        except Exception as e:
            pass

        sleep(random.uniform(2,3))

        # click on publish button
        # driver.find_element(By.XPATH, '//button[text()="publish"]').click()
        print('should click on publish button')
        sleep(500)



def main():
    # start = perf_counter()
    # # grab data from config file
    # with driver() as wd:
    #     with open(f'{os.getcwd()}/configs/facebook.json') as configList:
    #         for config in json.load(configList):
    #             bot = FaceBookBot(wd, config)
    #             bot.listItem()
    # print('time in ms: {}'.format((perf_counter() - start) * 1000))

    start = perf_counter()
    # grab data from config file
    with driver() as wd:
        with open(f'{os.getcwd()}/configs/craigslist.json') as configList:
            for config in json.load(configList):
                bot = CraigslistBot(wd, config)
                bot.listItem()
    print('time in ms: {}'.format((perf_counter() - start) * 1000))



if __name__ == '__main__':
	main()













