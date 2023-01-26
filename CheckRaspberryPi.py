import platform

if platform.system() == "Linux":
    import linux_tools as tools
elif platform.system() == "Windows":
    import windows_tools as tools

from concurrent.futures import ProcessPoolExecutor
import time
from requests_futures.sessions import FuturesSession
from bs4 import BeautifulSoup
import pandas as pd
import os
import sys
sys.path.append(tools.AUTOMATE_EMAIL_PATH)
from EmailingTool import EmailTool

# expand all: ctrl m + j
# collapse all: ctrl k + 0

os.chdir(os.path.dirname(__file__))

messages = []

def microCenter(text):
    soup = BeautifulSoup(text, 'html.parser')
    inventoryCnt = soup.find('span', {'class': 'inventoryCnt'})
    # print(f'{inventoryCnt.text = }')
    if inventoryCnt:
        messages.append(f'microcenter: {inventoryCnt.text}')
    # print([content for content in inventoryCnt.contents if isinstance(content, str)])

def vilros(text):
    soup = BeautifulSoup(text, 'html.parser')
    if "Sold Out" in ''.join(soup.strings).strip():
        # print('sold out in vilros stores')
        messages.append('sold out in vilros stores')
    else:
        # print('in stock in vilros stores')
        messages.append('in stock in vilros stores')


    # print(''.join(soup.strings).strip())

def pishop(text):
    soup = BeautifulSoup(text, 'html.parser')
    val = soup.find('input', attrs={'value':'Out of stock'})
    if val:
        # print(f"status: {val.get('value')}")
        # print('out of stock at piship!')
        messages.append('out of stock at piship!')
    else:
        # print('in stock at pi shop!')
        messages.append('in stock at pi shop!')

def adafruit(text):
    soup = BeautifulSoup(text, 'html.parser')
    val = soup.find('div', attrs={'itemprop': 'availability'})
    if val:
        # print(f"status: {val.text}")
        # print('out of stock at adafruit!')
        messages.append('out of stock at adafruit!')
    else:
        # print('in stock at adafruit!')
        messages.append('in stock at adafruit!')


def cankit(text):
    soup = BeautifulSoup(text, 'html.parser')
    # print(soup.find('table', attrs={'class': 'pdtHeadTable'}).text)
    desiredTable = soup.find('table', attrs={'class': 'pdtHeadTable'})
    # print(desiredTable)
    for row in desiredTable.find_all('tr'):
        if 'Pi 4 8GB Starter Kit - 32GB' in row.text:
            if 'Sold Out' in row.text:
                # print('sold out in cankit stores')
                messages.append('sold out in cankit stores')
            else:
                # print('in stock at cankit stores!')
                messages.append('in stock at cankit stores!')

def chicagoDist(text):
    soup = BeautifulSoup(text, 'html.parser')
    if "Sold Out" in ''.join(soup.strings).strip():
        # print('sold out in chicago dist stores')
        messages.append('sold out in chicago dist stores')
    else:
        # print('in stock at chicago dist stores')
        messages.append('in stock at chicago dist stores')


def main():
    lastDate = ''
    if not os.path.isfile(f'{os.getcwd()}/time_stamp.txt'):
        print('creating file')
        with open(f'{os.getcwd()}/time_stamp.txt', 'w') as f:
            f.write('')
    with open(f'{os.getcwd()}/time_stamp.txt', 'r') as f:
        lastDate = f.read()
        if lastDate == time.strftime("%Y-%m-%d") and tools.IS_CONNECTED_TO_NETWORK:
            print(f'{__file__}: already ran this')
            return
    with open(f'{os.getcwd()}/time_stamp.txt', 'w') as f:
        f.write(time.strftime("%Y-%m-%d"))

    urls = [
        'https://www.microcenter.com/product/621439/raspberry-pi-4-model-b---2gb-ddr4?src=raspberrypi',
        'https://vilros.com/products/raspberry-pi-4-2gb-ram?src=raspberrypi#',
        'https://www.pishop.us/product/raspberry-pi-4-model-b-2gb/?src=raspberrypi',
        'https://www.adafruit.com/product/4564',
        'https://www.canakit.com/raspberry-pi-4-8gb.html',
        'https://chicagodist.com/products/raspberry-pi-4-model-b-8gb?src=raspberrypi',
    ]

    # make sure the order of the funcs values and those of the urls matches (ORDER MATTERS)
    funcs = [microCenter,vilros,pishop,adafruit,cankit,chicagoDist,]

    # start = time.perf_counter()
    # with requests.Session() as session:
    #     print(list(map(session.get, urls)))
    # print(f'total for vanilla requests: {time.perf_counter() - start}')

    res = []

    start = time.perf_counter()
    with FuturesSession(executor=ProcessPoolExecutor(max_workers=8)) as session:
        futures = [session.get(url) for url in urls]
        for future in futures:
            future.result()
            res.append(future.result().text)

    # print(f'total for futures requests: {time.perf_counter() - start}')
    # print(len(futures))

    for text, func in zip(res, funcs):
        func(text)

    # print(messages)

    EmailTool.sendEmail('','', ['leozhang12345678@gmail.com'], 'Raspberry_pi_results', '\n'.join(messages) + f'\n{time.strftime("%Y-%m-%d")}')

if __name__ == '__main__':
    main()
