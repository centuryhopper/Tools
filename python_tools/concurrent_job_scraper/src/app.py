import asyncio
from playwright.async_api import Page, async_playwright
from dotenv import load_dotenv
import os

URL = 'https://storejobapplication.publix.com'

load_dotenv('secrets.env')

async def work(page: Page):
    
    await page.locator("a[data-qa='returning-applicant']").click()
    
    await page.wait_for_selector("text=Log in")
    
    await page.fill("#signInName", str(os.getenv('USERNAME')))
    await page.fill("#password", str(os.getenv('PASSWORD')))
    await page.click("#next")
    
    await page.wait_for_selector("text=Welcome back, Leo", timeout=600_000)  # 10 minutes

    # await page.pause()
    
    await page.wait_for_timeout(600_000)  # 10 minutes



# async def main():
def main():
    print("test")
    # async with async_playwright() as p:
    #     browser = await p.firefox.launch(headless=False)
    #     page = await browser.new_page()
    #     await page.goto(URL)

    #     await page.wait_for_timeout(3000)  # 3 seconds
        
    #     await work(page)
        
    #     await browser.close()

# asyncio.run(main())
# main()