import uiautomator2 as u2

from product_price_tracker.bot import *


class BestBuyBot(MobileBot):
    def __init__(self, package, device_serial, phone_pin) -> None:
        super().__init__(package, device_serial, phone_pin=phone_pin)

    def scrape(
        self,
        p: ProductInfo,
    ) -> float:
        self.human_delay(1,2)
        self.d.xpath("//*[@text='Search Best Buy']").click()

        self.d(className="android.widget.EditText").wait(timeout=5)
        self.human_delay(3,6)
        self.d(focused=True).send_keys(p.product_id)
        self.d.press("enter")
        self.human_delay(3, 6)
        self.scroll_down()

        price = self.d(resourceId="price-block-customer-price")
        if not price.wait(timeout=10):
            raise RuntimeError(f"price not found for {p.product_name}")

        desc = price.info["contentDescription"]
        return float(desc.replace("$", "").replace(",", ""))
