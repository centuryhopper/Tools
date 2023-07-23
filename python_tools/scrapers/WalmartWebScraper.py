#!/usr/bin/env python
# coding: utf-8

# TODO: use undetected chrome driver
import undetected_chromedriver as uc
import requests
import random
import re
from time import sleep, strftime
from bs4 import BeautifulSoup as soup



HEADERS = {"User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/97.0.4692.99 Safari/537.36", 'Accept-Language': 'en-US, en;q=0.5'}

# TODO: Look into using this url: https://www.walmart.com/shop/deals?page=16 with selenium
# stop using requests and bs4 as websites can easily block bots that simply use such an architecture

html = requests.get('https://www.walmart.com/browse/personal-care/hand-soap/1005862_1001719?page=1',headers=HEADERS)


bsobj = soup(html.content,'html.parser')

url_list = []
for i in range(1,26):
    url_list.append('https://www.walmart.com/browse/personal-care/hand-soap/1005862_1001719?page=' + str(i))

item_names = []
price_list = []
item_ratings = []
item_reviews = []

for url in url_list:
    result = requests.get(url, headers=HEADERS)
    bsobj = soup(result.content,'html.parser')

    product_name = bsobj.findAll('span',{'class':'f6 f5-l normal dark-gray mb0 mt1 lh-title'})

    ratings_and_reviews_elems = bsobj.findAll('div', {'mt2 flex items-center'})

    ratings_and_reviews = [e.find('span',{'class':'w_DT'}).text for e in ratings_and_reviews_elems]
    ratings = [re.split('(?<=[A-Za-z])\.', s)[0].strip() for s in ratings_and_reviews]
    reviews = [re.split('(?<=[A-Za-z])\.', s)[1].strip() for s in ratings_and_reviews]

    product_price = bsobj.findAll('div',{'class':'flex flex-wrap justify-start items-center lh-title mb2 mb1-m'})
    product_price = [e.div.text.strip() for e in product_price]

    for names,rating,reviews,price in zip(product_name,ratings,reviews,product_price):
        item_names.append(names.text.strip())
        item_ratings.append(rating)
        item_reviews.append(reviews)
        price_list.append(price)
    sleep(random.uniform(0.1, 0.5))


# creating a dataframe
import pandas as pd
df = pd.DataFrame({'Product_Name':item_names, 'Price':price_list, 'Rating':item_ratings,'No_Of_Reviews':item_reviews}, columns=['Product_Name', 'Price', 'Rating', 'No_Of_Reviews'])
# df.head()
df.to_csv(f"data/data_{strftime('%Y-%m-%d-%H-%M-%S')}.csv")










