import os
import time
from appium import webdriver

# npm i -g appium (if haven't done so already)
# start the server first: appium --address localhost --port 4723
# or just appium
# which would default the address to 0.0.0.0 and port 4723

desired_caps = {
    "platformName": "Android",
    "platformVersion": "11",
    "deviceName": "test_emulator",
    "appPackage": "",
    "appActivity": "",
    "automationName": "UiAutomator2",
    "connectionType": "adb"
}

driver = webdriver.Remote('http://0.0.0.0:4723/wd/hub', desired_caps)


'''
TODO:

cron shell script:

emulator -avd test_emulator -no-window -no-audio &

echo "Waiting for emulator to start up..."
adb wait-for-device

# Start the Appium server
appium &

The & symbol is a shell operator that runs the preceding command in the background. In the case of running appium &, it starts the Appium server in the background and returns control to the command prompt immediately, allowing you to continue working in the terminal without waiting for Appium to finish running.

# Wait for the Appium server to start up
sleep 10

# Run the Python Appium script
python /path/to/your/script.py

'''

# Find an element by ID and click on it
#element = driver.find_element_by_id("my_button")
#element.click()
#
## Find an element by text and enter some text into it
#element = driver.find_element_by_android_uiautomator('new UiSelector().text("Enter your name")')
#element.send_keys("John Doe")
#
## Swipe left on an element
#element = driver.find_element_by_id("my_swipeable_element")
#driver.swipe(start_x, start_y, end_x, end_y)

# Take a screenshot
driver.save_screenshot(f"{os.getcwd()}/appium_screenshots/screenshot_{time.strftime('%Y_%m_%d_%H_%M_%S')}.png")


driver.quit()

