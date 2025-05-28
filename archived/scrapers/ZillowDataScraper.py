#!/usr/bin/env python
# coding: utf-8

# Webscraping Zillow

import concurrent.futures
import os
import random
from pathlib import Path
from time import sleep, strftime

import pandas as pd
import undetected_chromedriver as uc
from bs4 import BeautifulSoup as soup
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.wait import WebDriverWait

os.chdir(os.path.dirname(__file__))


def getAllPageLinks(url):
	userAgent = 'Mozilla/5.0 (X11; Linux x86_64; rv:107.0) Gecko/20100101 Firefox/107.0'
	options = webdriver.ChromeOptions()
	# options.add_argument('--headless')
	options.add_argument('proxy-server=106.122.8.54:3128')
	options.add_argument(f'user-agent={userAgent}')
	driver = uc.Chrome(options=options)
	driver.get(url)
	driver.implicitly_wait(5)

	sleep(random.uniform(10,12))

	div = driver.find_element(By.CLASS_NAME, 'search-pagination')

	navChild = div.find_element(By.XPATH, '//nav[contains(@aria-label, "Pagination")]')

	atags = navChild.find_elements(By.TAG_NAME, 'a')

	# for atag in atags:
	# 	if atag:
	# 		print(atag.get_attribute('title'))

	pageLinks = [atag.get_attribute('href') for atag in atags if atag and atag.get_attribute('title').startswith('Page')]

	# print(pageLinks)

	driver.quit()

	return pageLinks

def getDataOnPage_ThreadWork(url):
	options = webdriver.ChromeOptions()
	# options.add_argument('--headless')
	# options.add_argument('proxy-server=106.122.8.54:3128')
	driver = uc.Chrome(options=options)
	driver.get(url)

	sleep(random.uniform(2,3))

	# print(driver.page_source)

	# scroll to bottom of page
	driver.execute_script("window.scrollTo(0, document.body.scrollHeight)")
	sleep(random.uniform(2,3))

	ul = driver.find_element(By.XPATH, "//ul[contains(@class, 'photo-cards with_constellation')]")

	prices = ul.find_elements(By.XPATH, '//span[contains(@data-test, "property-card-price")]')

	links = ul.find_elements(By.XPATH, '//a[contains(@data-test, "property-card-link")]')

	addresses = ul.find_elements(By.XPATH, '//address[contains(@data-test, "property-card-addr")]')

	priceLst = []
	addressLst = []
	linksLst = []

	# print(f'number of prices: {len(prices)}')
	# print(f'number of links: {len(links)}')
	# print(f'number of addresses: {len(addresses)}')

	for price,link,address in zip(prices,links, addresses):
		priceLst.append(price.text)
		linksLst.append(link.get_attribute('href'))
		addressLst.append(address.text)
		# print(price.text)
		# print(link.get_attribute('href'))
		# print(address.text)
		# print('========================================')

	d = {
		'price': priceLst,
		'address': addressLst,
		'links': linksLst
	}

	df = pd.DataFrame(d)
	df.index.name = 'id'
	# print(df)
	# sleep(10000)
	driver.quit()
	return df

if __name__ == "__main__":
	city_state = 'Lakeland-FL'
	# city_state = 'tallahassee-FL'
	# city_state = 'Melbourne-FL'
	url = f'https://www.zillow.com/homes/for_rent/{city_state}'
	links = getAllPageLinks(url)
	ans = None
	# multi-threading
	with concurrent.futures.ThreadPoolExecutor() as executor:
		res = executor.map(getDataOnPage_ThreadWork,links)
		# list of dataframes
		ans = list(res)

	final_df = pd.concat(ans)
	final_df.index.name = 'id'
	print(final_df)
	output_folder = Path.cwd() / 'data'
	output_folder.mkdir(exist_ok=True)
	final_df.to_csv(f"{os.getcwd()}/data/zillow_{city_state}_{strftime('%Y-%m-%d-%H-%M-%S')}.csv")






