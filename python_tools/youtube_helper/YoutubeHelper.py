# code inspired by https://www.youtube.com/watch?v=GpDQpiCiNZ8&ab_channel=TopNotchProgrammer

# Prerequistes mentioned in the video:
# In settings:
#   - make sure videos are never made for kids
import platform

if platform.system() == "Linux":
    import linux_tools as tools
elif platform.system() == "Windows":
    import windows_tools as tools

import json
import os
import random
from time import sleep

import undetected_chromedriver as uc
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By

os.chdir(os.path.dirname(__file__))

videos_path = f'{os.getcwd()}/videos'

configData = None
with open(f"{os.getcwd()}/config.json") as configFile:
    configData = json.load(configFile)

keys = configData.keys()

videos = os.listdir(videos_path)

for key, video_file in zip(keys, videos):
    chrome_options = Options()
    # options.add_experimental_option('excludeSwitches', ['enable-logging'])
    chrome_options.add_argument("--log-level=3")
    chrome_options.set_capability("detach", True)
    # which chrome profile to use
    chrome_options.add_argument('--profile-directory=Default')
    # directory containing all the profiles
    chrome_options.add_argument(f"--user-data-dir={tools.CHROME_DATA_PATH}")
    chrome_options.binary_location = tools.CHROME_BINARY_LOCATION
    bot = uc.Chrome(options=chrome_options)
    bot.get("https://studio.youtube.com")
    sleep(3)
    upload_button = bot.find_element(By.XPATH, '//*[@id="upload-icon"]')
    upload_button.click()
    sleep(1)

    file_input = bot.find_element(By.XPATH, '//*[@id="content"]/input')
    file_input.send_keys(os.path.join(videos_path, video_file))

    sleep(random.uniform(7,10))

    inputs = bot.find_elements(By.XPATH, '//*[@id="textbox"]')

    # assuming that inputs always return two elements is kind of dangerous
    # but I've tested many times and there's always two elements returned
    title, desc = inputs

    title.clear()
    title.send_keys(configData[key]['title'])
    desc.clear()
    desc.send_keys(configData[key]['description'])

    sleep(random.uniform(5,7))

    next_button = bot.find_element(By.XPATH, '//*[@id="next-button"]')
    for i in range(3):
        next_button.click()
        sleep(1)

    done_button = bot.find_element(By.XPATH, '//*[@id="done-button"]')
    done_button.click()
    sleep(5)
    bot.close()

    print('finished with this video')
    # sleep(5000)





