import random
from abc import ABC, abstractmethod
from enum import Enum
from time import sleep

import uiautomator2 as u2
from playwright.async_api import BrowserContext


class PLATFORM(Enum):
    BESTBUY = 0
    WALMART = 1
    NEWEGG = 2
    AMAZON = 3


class ProductInfo:
    def __init__(
        self,
        url: str,
        platform: PLATFORM,
        product_name: str,
        product_id: str,
    ) -> None:
        self.url = url
        self.platform = platform
        self.product_name = product_name
        self.product_id = product_id


class WebBot(ABC):
    def __init__(self) -> None:
        pass

    @abstractmethod
    async def scrape(
        self,
        product: ProductInfo,
        context: BrowserContext,
    ) -> float:
        pass


class MobileBot(ABC):
    def __init__(self, package, device_serial, phone_pin: str | None = None) -> None:
        self.package = package
        self.phone_pin = phone_pin
        self.d = u2.connect(device_serial)

        self.unlock()

        # set keyboard to adb keyboard
        self.d.shell("ime set com.github.uiautomator/.AdbKeyboard")

        self.min_delay = 1.5
        self.max_delay = 4.0

    def is_locked(self) -> bool:
        focus = self.d.shell("dumpsys window | grep mCurrentFocus").output
        return "OFF_LOCKED" in focus or "ON_LOCKED" in focus

    def unlock(self) -> None:
        self.d.shell("input keyevent KEYCODE_WAKEUP")
        self.d.shell(
            "wm dismiss-keyguard"
        )  # no lock: unlocks; PIN lock: shows the PIN pad
        sleep(1)

        if not self.is_locked():
            return
        if self.phone_pin is None:
            raise RuntimeError("Phone is PIN-locked but no PIN was provided")

        for d in list(self.phone_pin):
            self.d(text=d).click_exists()
        sleep(1)

        if self.is_locked():
            raise RuntimeError("Unlock failed (wrong PIN or lock screen not ready)")

    def lock(self) -> None:
        self.d.shell("input keyevent KEYCODE_SLEEP")

    def scroll_down(self):
        """makes the screen go down"""
        width, height = self.d.window_size()

        # width // 2	fx	Start X: horizontal center of the screen
        # int(height * 0.80)	fy	Start Y: 80% of the way down the screen, near the bottom
        # width // 2	tx	End X: still the horizontal center, so the swipe is perfectly vertical
        # int(height * 0.20)	ty	End Y: 20% of the way down, near the top
        # duration=0.4	duration	How long the swipe takes, in seconds (400 ms)
        self.d.swipe(
            width // 2,
            int(height * 0.80),
            width // 2,
            int(height * 0.20),
            duration=0.4,
        )

        self.human_delay(0.5, 1)

    def human_delay(self, min_sec=None, max_sec=None):
        """Random delay to mimic human reaction time"""
        min_sec = min_sec or self.min_delay
        max_sec = max_sec or self.max_delay
        sleep(random.uniform(min_sec, max_sec))

    def launch_app(self):
        """Start app with random startup delay"""
        self.d.app_start(self.package)
        # Wait for app to fully load (varies by device)
        self.human_delay(3, 6)

    def close(self):
        """Clean exit"""
        self.d.app_stop(self.package)

    @abstractmethod
    def scrape(
        self,
        p: ProductInfo,
    ) -> float:
        pass
