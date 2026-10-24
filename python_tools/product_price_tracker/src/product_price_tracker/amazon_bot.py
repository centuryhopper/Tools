import asyncio

from playwright.async_api import BrowserContext

from product_price_tracker.bot import WebBot, ProductInfo


class AmazonBot(WebBot):
    def __init__(self) -> None:
        super().__init__()

    async def scrape(
        self,
        product: ProductInfo,
        context: BrowserContext,
    ) -> float:
        await asyncio.sleep(1)
        return float()
