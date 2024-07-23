import pwd
import os
import subprocess

AUTOMATE_TEXTING_PATH = f'/home/{pwd.getpwuid(os.getuid()).pw_name}/Documents/GitHub/automate_texting/'
LOG_PATH = '/tmp/'
BASE_DIR = f'/home/{pwd.getpwuid(os.getuid()).pw_name}/Documents/GitHub/'
PATH_TO_WALLPAPERS = f'/home/{pwd.getpwuid(os.getuid()).pw_name}/custom_wallpapers/Community-wallpapers/merged/'
CHROME_DATA_PATH = f'/home/{pwd.getpwuid(os.getuid()).pw_name}/.config/google-chrome/'
CHROME_BINARY_LOCATION = "/opt/google/chrome/google-chrome"

WIFI_CHECK = subprocess.run(['curl', '-Is', 'https://www.google.com'], capture_output=True, text=True).stdout
IS_CONNECTED_TO_NETWORK = 'HTTP/2 200' in WIFI_CHECK

AUTOMATE_EMAIL_PATH = f'/home/{pwd.getpwuid(os.getuid()).pw_name}/Documents/GitHub/Tools/EmailTool/'

