# -*- coding: utf-8 -*-

"""
Created on 5/21/2025
Converted to Playwright Async on 2026-04-10

Title: get_youtube_transcript.py

Description: get transcript (auto-generated) from any youtube video.
"""

import os
import pandas as pd
from time import strftime
from typing import Optional

from playwright.async_api import async_playwright


os.chdir(os.path.dirname(__file__))

LOG_PATH = "./logs"
os.makedirs(LOG_PATH, exist_ok=True)

PATH_TO_TRANSCRIPT = "./yt_transcripts"
os.makedirs(PATH_TO_TRANSCRIPT, exist_ok=True)


# -----------------------------
# Browser setup
# -----------------------------
async def open_browser(url: str, mode: str = "headed"):
    playwright = await async_playwright().start()

    browser = await playwright.firefox.launch(
        headless=(mode == "headless")
    )

    context = await browser.new_context(
        viewport={"width": 1280, "height": 800},
        locale="en-US",
    )

    page = await context.new_page()
    await page.goto(url)

    return playwright, browser, context, page


# -----------------------------
# Handle transcript UI
# -----------------------------
async def get_transcript(page):
    await page.reload()
    await page.wait_for_timeout(3000)

    print("Opening transcript...")

    # Try clicking "more"
    try:
        more_btn = page.locator("#expand")
        if await more_btn.count() > 0:
            await more_btn.first.click()
    except:
        pass

    # Try clicking "Show transcript"
    try:
        show_btn = page.locator("button[aria-label='Show transcript']")
        if await show_btn.count() > 0:
            await show_btn.first.click()
    except:
        pass

    await page.wait_for_timeout(3000)

    # Grab transcript panel
    try:
        panel = page.locator("#panels")
        text = await panel.inner_text()
        return text
    except:
        return ""


# -----------------------------
# Convert transcript -> dataframe
# -----------------------------
def transcript2df(transcript: str):
    lines = transcript.split("\n")

    timestamps = lines[1::2]
    text = lines[::2]

    min_len = min(len(timestamps), len(text))

    df = pd.DataFrame({
        "timestamp": timestamps[:min_len],
        "text": text[:min_len],
    })

    return df


# -----------------------------
# Main flow
# -----------------------------
async def main(url: str, mode: str = "headless"):
    playwright, browser, context, page = await open_browser(url, mode)

    try:
        transcript = await get_transcript(page)
        df = transcript2df(transcript)

        print("Saving transcript...")

        ts = strftime('%Y-%m-%d-%H_%M_%S')

        df.to_csv(
            os.path.join(PATH_TO_TRANSCRIPT, f"transcript_{ts}.csv"),
            index=False,
        )

        with open(
            os.path.join(PATH_TO_TRANSCRIPT, f"transcript_{ts}.txt"),
            "w",
            encoding="utf-8",
        ) as f:
            f.write(" ".join(df["text"].tolist()))

        print(f"Saved to {PATH_TO_TRANSCRIPT}")

    finally:
        await browser.close()
        await playwright.stop()


# -----------------------------
# CLI
# -----------------------------
if __name__ == "__main__":

    while "https://www.youtube.com/watch?v=" not in (
        url := input("Enter YouTube URL: ")
    ):
        print("Invalid URL, try again.")

    while (
        mode := input("1 = headed, 2 = headless: ")
    ) not in ["1", "2"]:
        print("Try again.")

    import asyncio

    asyncio.run(main(url, "headed" if mode == "1" else "headless"))