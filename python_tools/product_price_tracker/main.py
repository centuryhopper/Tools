import asyncio
import os
from typing import List

import uiautomator2 as u2
from dotenv import load_dotenv
from playwright.async_api import BrowserContext, async_playwright
from playwright_stealth import Stealth

from product_price_tracker.amazon_bot import AmazonBot
from product_price_tracker.bestbuy_bot import BestBuyBot
from product_price_tracker.bot import PLATFORM, MobileBot, ProductInfo, WebBot

load_dotenv('secrets.env')

amzn_bot = AmazonBot()
bestbuy_bot = BestBuyBot(
    package="com.bestbuy.android", device_serial=os.getenv("GALAXY_S26")
)


# prefer the browser and use mobile as a fallback if the website blocks browser
async def web_scrape_dispatcher(
    product: ProductInfo,
    context: BrowserContext,
) -> float:
    match product.platform:
        case PLATFORM.AMAZON:
            return await amzn_bot.scrape(product, context)
        case _:
            raise ValueError(f"No scraper for platform {product.platform}")

# http://127.0.0.1:33299
def mobile_scrape_dispatcher(product: ProductInfo, bot: MobileBot) -> float:
    try:
        bot.launch_app()
        value = bot.scrape(product)
    finally:
        bot.close()
    return value


async def main():
    browser_set = set(
        [
            PLATFORM.AMAZON,
        ]
    )
    mobile_set = set(
        [
            PLATFORM.BESTBUY,
        ]
    )
    products: List[ProductInfo] = [
        ProductInfo(
            url="https://www.bestbuy.com/product/gigabyte-aero-x16-copilot-pc-16-25601600-wqxga-amd-ryzen-al-7-350-1tb-ssd-32gb-ddr5-ram-geforce-rtx-5070-space-gray/J3GWPQCCFK/sku/6632266",
            platform=PLATFORM.BESTBUY,
            product_name="AERO X16 5070",
            product_id="6632266",
        ),
        # ProductInfo(
        #     url="https://www.bestbuy.com/product/gigabyte-aero-x16-copilot-pc-16-25601600-wqxga-amd-ryzen-al-7-350-1tb-ssd-32gb-ddr5-ram-geforce-rtx-5060-space-gray/J3GWPQCXYT",
        #     platform=PLATFORM.BESTBUY,
        #     product_name="AERO X16 5060",
        #     product_id="6635752",
        # ),
    ]

    browser_products = [p for p in products if p.platform in browser_set]
    mobile_products = [p for p in products if p.platform in mobile_set]

    if browser_products:
        # Every context and page created inside this block gets the stealth patches.
        async with Stealth().use_async(async_playwright()) as p:
            browser = await p.chromium.launch(channel="chrome", headless=False)
            context = await browser.new_context()
            try:
                results = await asyncio.gather(
                    *(web_scrape_dispatcher(p, context) for p in browser_products),
                    return_exceptions=True,
                )
            finally:
                await browser.close()
        for p, exception in zip(browser_products, results):
            print(
                f"{'ERROR: ' + str(exception) if isinstance(exception, Exception) else exception}\n  {p}\n"
            )

    for p in mobile_products:
        match p.platform:
            case PLATFORM.BESTBUY:
                mobile_scrape_dispatcher(p, bestbuy_bot)
            case _:
                raise ValueError(f"No scraper for platform {p.platform}")


if __name__ == "__main__":
    asyncio.run(main())
