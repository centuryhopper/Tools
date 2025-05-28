# !/usr/bin/python3

from instabot import Bot
from dotenv import load_dotenv
import os

load_dotenv()


bot = Bot()
bot.login(username=os.getenv("ig_user"), password=os.getenv("ig_pass"))
bot.upload_photo("./test.jpeg")
