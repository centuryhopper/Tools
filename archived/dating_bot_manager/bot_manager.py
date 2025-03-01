# the line below avoids auto generating a __pycache__ directory when this script is running
import sys
sys.dont_write_bytecode = True

import os
from tinder_bot import TinderBot
from dotenv import load_dotenv

os.chdir(os.path.dirname(__file__))

# Load environment variables from the .env file
load_dotenv('secrets.env')



if __name__ == '__main__':
    account_number = input('pick an account to run (type 1 or 2):\n')
    if account_number not in list('12'):
        raise Exception('please enter 1 or 2')
    account_number = int(account_number)
    username = ''
    password = ''
    unique_name = ''
    match account_number:
        case 1:
            username,password,unique_name = os.getenv('account1').split(',')
        case 2:
            username,password,unique_name = os.getenv('account2').split(',')
        case _:
            raise Exception("something went wrong...")
    
    t = TinderBot(username, password, unique_name, configs_path="./configs")
    # t.login()
    print('auto swiping...')
    t.auto_swipe()
    t.dispose()
    
    
    