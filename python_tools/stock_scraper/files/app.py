#!/home/leo_zhang/miniconda3/envs/ai/bin/python


# -*- coding: utf-8 -*-
"""
Created on 5/4/2025
@author: Leo Zhang


Description: Compares the price of a list of stocks to their previous 200 day simple moving average (SMA)

"""

import yfinance as yf
import matplotlib.pyplot as plt
import pandas as pd
import sys

# Choose your stock symbol, e.g., 'AAPL' for Apple
'''
SymbolCompany% Assets
AAPL
Apple Inc. 7.00%
MSFT
Microsoft Corporation 5.85%
NVDA
NVIDIA Corporation 5.56%
AMZN
Amazon.com, Inc. 3.76%
META
Meta Platforms, Inc. 2.65%
BRK-B
Berkshire Hathaway Inc. 2.05%
GOOGL
Alphabet Inc. 1.89%
AVGO
Broadcom Inc. 1.65%
GOOG
Alphabet Inc.
TSLA
Tesla, Inc.
'''

args = sys.argv[1:]

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

# Download 1 year of daily data (more than 200 days to calculate SMA)
data: pd.DataFrame = yf.download(symbols, period="1y")

# Calculate 200-day simple moving average
data["200_SMA"] = data["Close"].rolling(window=200).mean()

last_row_series = data["200_SMA"].tail(1)

# TODO: send email out to me if current price is lower than moving average
# Do so by composing an email body text. Include the ticker symbol along with its current price and most recent moving average

def build_email_body():
    lst = []
    # if current < most recent moving average
        # then add text to list f"{ticker}'s current price is {cur_price}. The most recent moving average over the last 200 days is {last_200_days_SMA}"


def get_current_price(ticker: str):
    # Load stock
    stock = yf.Ticker(ticker)
    # Get current price from the latest market data
    current_price = stock.history(period="1d")["Close"].iloc[-1]
    print(f"Current price of {ticker}: ${current_price:.2f}")



import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
import ssl

def send_email():
    sender_email = "your_email@gmail.com"
    receiver_email = "your_email@gmail.com"
    password = "your_app_password"  # Use the generated App Password here

    subject = "Test Email from Python"
    body = "This is a test email sent from Python."

    # Create the email content
    msg = MIMEMultipart()
    msg['From'] = sender_email
    msg['To'] = receiver_email
    msg['Subject'] = subject
    msg.attach(MIMEText(body, 'plain'))

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
    pass
