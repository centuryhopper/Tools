#!/home/leo_zhang/miniconda3/envs/web_auto/bin/python

import time
from binance.client import Client
import os
from dotenv import load_dotenv, find_dotenv
from pprint import pprint

load_dotenv()

API_Key = os.getenv('API_Key')
Secret_Key = os.getenv('Secret_Key')


client = Client(API_Key, Secret_Key, testnet=True)

# pprint(client.get_account())

symbol = 'BTCUSDT'
buy_price_threshold = 96598
sell_price_threshold = 96605
trade_quantity = 0.001

def get_current_price(tickerSymbol):
    ticker = client.get_symbol_ticker(symbol=tickerSymbol)
    return float(ticker['price'])

print(f'current price of {symbol}: {get_current_price(symbol)}')

def place_buy_order(tickerSymbol, quantity):
    order = client.order_market_buy(symbol=tickerSymbol, quantity=quantity)
    print(f'Buy order done: {order}')
    
# place_buy_order(symbol, trade_quantity)
# print(client.get_account())

def place_sell_order(tickerSymbol, quantity):
    order = client.order_market_sell(symbol=tickerSymbol, quantity=quantity)
    print(f'Sell order done: {order}')
# place_sell_order(symbol, trade_quantity)
# place_sell_order(symbol, trade_quantity)

def trading_bot():
    in_position = False
    
    while True:
        current_price = get_current_price(symbol)
        print(f"Current price of {symbol}: {current_price}")

        if not in_position:
            if current_price < buy_price_threshold:
                print(f"Price is below {buy_price_threshold}. Placing buy order.")
                place_buy_order(symbol, trade_quantity)
                in_position = True
        else:
            if current_price > sell_price_threshold:
                print(f"Price is above {sell_price_threshold}. Placing sell order.")
                place_sell_order(symbol, trade_quantity)
                in_position = False
        time.sleep(3)


if __name__ == "__main__":
    trading_bot()