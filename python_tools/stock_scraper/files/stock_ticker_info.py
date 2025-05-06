

class stock_ticker_info:
    def __init__(self, ticker_symbol:str, cur_price: float, last_200_days_SMA: float) -> None:
        self.ticker_symbol = ticker_symbol
        self.cur_price = cur_price
        self.last_200_days_SMA = last_200_days_SMA