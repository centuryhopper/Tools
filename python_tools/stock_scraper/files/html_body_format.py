from typing import List

from stock_ticker_info import stock_ticker_info


def build_html_body(symbols: List[stock_ticker_info]) -> str:
    html_rows = ""
    for symbol in symbols:
        ...
        html_rows += f"""
            <tr>
                <td>{symbol.ticker_symbol}</td>
                <td>${symbol.cur_price:.2f}</td>
                <td>${symbol.last_200_days_SMA:.2f}</td>
            </tr>
        """
    html_body = f"""
        <html>
            <head>
                <style>
                    table {{
                        font-family: Arial, sans-serif;
                        border-collapse: collapse;
                        width: 100%;
                    }}
                    td, th {{
                        border: 1px solid #dddddd;
                        text-align: center;
                        padding: 8px;
                    }}
                    th {{
                        background-color: #f2f2f2;
                    }}
                </style>
            </head>
            <body>
                <h2>Stock Price vs 200-Day Moving Average</h2>
                <table>
                    <tr>
                        <th>Symbol</th>
                        <th>Current Price</th>
                        <th>200-Day SMA</th>
                    </tr>
                    {html_rows}
                </table>
            </body>
        </html>
    """
    return html_body