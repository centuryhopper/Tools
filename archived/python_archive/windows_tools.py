import os
import subprocess

AUTOMATE_TEXTING_PATH = f'C:/Users/{os.getlogin()}/Documents/GitHub/automate_texting/'
LOG_PATH = 'C:/Windows/Temp'
BASE_DIR = f'C:/Users/{os.getlogin()}/Documents/GitHub/'
PATH_TO_WALLPAPERS = ''
CHROME_DATA_PATH = ''
CHROME_BINARY_LOCATION = ''
AUTOMATE_EMAIL_PATH = f'C:/Users/{os.getlogin()}/Documents/GitHub/Tools/EmailTool/'

WIFI_CHECK = subprocess.check_output('netsh interface show interface | findstr "Wi-Fi"', shell=True).decode()
IS_CONNECTED_TO_NETWORK = 'Connected' in WIFI_CHECK
