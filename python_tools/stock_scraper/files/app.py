#!/home/leo_zhang/miniconda3/envs/ai/bin/python


# -*- coding: utf-8 -*-
"""
Created on 5/4/2025
@author: Leo Zhang


Description: Compares the price of a list of stocks to their previous 200 day simple moving average (SMA)

"""

import os
import sys
from typing import List, Union

import matplotlib.pyplot as plt
import pandas as pd
import yfinance as yf
from dotenv import load_dotenv

from html_body_format import build_html_body
from stock_ticker_info import stock_ticker_info

load_dotenv()
# Choose your stock symbol, e.g., 'AAPL' for Apple
'''
Symbol Company
AAPL   Apple Inc.
MSFT   Microsoft Corporation
NVDA   NVIDIA Corporation
AMZN   Amazon.com, Inc.
META   Meta Platforms, Inc.
BRK-B  Berkshire Hathaway Inc.
GOOGL  Alphabet Inc.
AVGO   Broadcom Inc.
GOOG   Alphabet Inc.
TSLA   Tesla, Inc.
'''

def get_current_price(ticker: str) -> float:
    # Load stock
    stock = yf.Ticker(ticker)
    # Get current price from the latest market data
    current_price = stock.history(period="1d")["Close"].iloc[-1]
    print(f"Current price of {ticker}: ${current_price:.2f}")
    return float(current_price)

# send email out to me if current price is lower than moving average
# the method does so by composing an email body text. Include the ticker symbol along with its current price and most recent moving average

def build_email_body() -> str:
    args = sys.argv[1:]
    # make sure each ticker symbol is in all caps
    args = [arg.upper() for arg in args]
    # top 10 stocks in S & P 500
    symbols = args if args else [
        'AAPL',
        'MSFT',
        'NVDA',
        'AMZN',
        'META',
        'BRK-B',
        'GOOGL',
        'AVGO',
        'GOOG',
        'TSLA',
    ]
    email_body_lst: List[stock_ticker_info] = []
    for symbol in symbols:
        # Download 1 year of daily data (more than 200 days to calculate SMA)
        data: pd.DataFrame | None = None
        try:
            data = yf.download(symbol, period="1y")
        except Exception as e:
            pass
        # print(f'data: {data}')
        if data is None or data.empty: continue
        # Calculate 200-day simple moving average
        data["200_SMA"] = data["Close"].rolling(window=200).mean()
        last_200_days_SMA = data["200_SMA"].tail(1).values
        last_200_days_SMA = float(last_200_days_SMA[0])
        # print(f"last_200_days_SMA for {symbol}: {last_200_days_SMA}")
        cur_price = get_current_price(symbol)
        # if cur_price < last_200_days_SMA:
        # email_body_lst.append(f"{symbol}'s current price is {cur_price}. The most recent moving average over the last 200 days is {last_200_days_SMA}")
        email_body_lst.append(stock_ticker_info(symbol, cur_price, last_200_days_SMA))
    return build_html_body(email_body_lst)

def send_email(subject:str, body:str, body_is_html: bool=False):
    import smtplib
    import ssl
    from email.mime.multipart import MIMEMultipart
    from email.mime.text import MIMEText
    sender_email :str | None = os.getenv('email')
    receiver_email :str | None = os.getenv('receiver')
    password :str | None = os.getenv('password')
    
    if not sender_email or not receiver_email or not password:
        raise Exception("values (email, receiver, and/or password) not assigned in the .env")

    # Create the email content
    msg = MIMEMultipart()
    msg['From'] = sender_email
    msg['To'] = receiver_email
    msg['Subject'] = subject
    
    # Attach body as HTML or plain text
    mime_type = 'html' if body_is_html else 'plain'
    msg.attach(MIMEText(body, mime_type))

    # Set up the secure SSL context
    context = ssl.create_default_context()

    try:
        # Establish a secure session with Gmail's SMTP server
        with smtplib.SMTP_SSL("smtp.gmail.com", 465, context=context) as server:
            server.login(sender_email, password)
            server.sendmail(sender_email, receiver_email, msg.as_string())
            print("Email sent successfully!")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    email_body = build_email_body()
    # print(email_body)
    send_email('Stocks that should be further researched', email_body, body_is_html=True)
