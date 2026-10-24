# Async Playwright in Python — Setup & Debugging

A quick reference for setting up Playwright's async API in Python and using its debugging tools.

---

## 1. Installation

With **uv**:

```bash
uv add playwright
uv run playwright install chromium
```

With **pip**:

```bash
pip install playwright
playwright install chromium
```

`playwright install` downloads the browser binaries Playwright drives. Use `chromium`, `firefox`, `webkit`, or omit the name to get all three.

On Linux, if the browser fails to launch because of missing system libraries, install them with:

```bash
playwright install --with-deps chromium
```

---

## 2. Minimal Async Script

```python
import asyncio
from playwright.async_api import async_playwright


async def main():
    async with async_playwright() as p:
        browser = await p.chromium.launch(headless=True)
        context = await browser.new_context()
        page = await context.new_page()

        await page.goto("https://example.com")
        print(await page.title())

        await browser.close()


if __name__ == "__main__":
    asyncio.run(main())
```

### How the pieces fit

| Object | What it is |
|---|---|
| `async_playwright()` | Starts the Playwright driver. Use it as an `async with` block so it shuts down cleanly. |
| `browser` | A browser process (Chromium, Firefox, or WebKit). Expensive — launch once, reuse. |
| `context` | An isolated browser session: its own cookies, storage, and cache. Cheap — make one per independent task. |
| `page` | A single tab inside a context. |

### Rules of the async API

- **Every Playwright call that touches the browser must be awaited:** `await page.click(...)`, `await page.fill(...)`, `await locator.text_content()`.
- **Locator creation is not awaited** — only the actions on it are:
  ```python
  button = page.get_by_role("button", name="Submit")  # no await
  await button.click()                                # await
  ```
- **Never mix `playwright.sync_api` and `playwright.async_api`** in the same program.
- **Don't call `asyncio.run()` inside an already-running event loop** (e.g. Jupyter). In a notebook, just `await main()` directly.

### Running pages concurrently

The main reason to use the async API is running multiple pages at once:

```python
async def scrape(context, url):
    page = await context.new_page()
    await page.goto(url)
    title = await page.title()
    await page.close()
    return title


async def main():
    urls = ["https://example.com", "https://example.org", "https://example.net"]
    async with async_playwright() as p:
        browser = await p.chromium.launch()
        context = await browser.new_context()
        titles = await asyncio.gather(*(scrape(context, u) for u in urls))
        print(titles)
        await browser.close()
```

To cap concurrency, wrap the work in an `asyncio.Semaphore`.

---

## 3. Debugging

### 3.1 Watch the browser (headed mode + slow motion)

The simplest debugging step: see what's happening.

```python
browser = await p.chromium.launch(headless=False, slow_mo=500)  # 500 ms between actions
```

### 3.2 Playwright Inspector

The Inspector is Playwright's built-in debugger. It opens a window next to the browser where you can **step through actions one at a time**, see which element each locator matches, and try out new locators live.

**Option A — debug the whole script from the start:**

```bash
PWDEBUG=1 python script.py
```

This forces headed mode, disables action timeouts (so you can sit on a step as long as you want), and pauses before the first action.

> In **fish**, the same `PWDEBUG=1 python script.py` syntax works (fish 3.1+). In PowerShell: `$env:PWDEBUG=1; python script.py`.

**Option B — pause at a specific point:**

```python
await page.goto("https://example.com/login")
await page.pause()  # Inspector opens here
await page.get_by_label("Email").fill("me@example.com")
```

`page.pause()` only does something in headed mode — launch with `headless=False` or run with `PWDEBUG=1`.

**Using the Inspector window:**

| Control | What it does |
|---|---|
| **Resume** (▶) | Run until the next `page.pause()` or the end of the script. |
| **Step over** | Execute the next Playwright action, then pause again. |
| **Pick locator** | Click any element in the browser to get a recommended locator for it. |
| **Locator field** | Type a locator (e.g. `get_by_role("button", name="Save")`) to highlight what it matches on the page. |
| **Record** | Records your clicks/typing as code you can copy into the script. |

### 3.3 Browser console helpers

```bash
PWDEBUG=console python script.py
```

Opens the browser with DevTools and exposes a `playwright` object in the DevTools console:

```js
playwright.$('text=Sign in')         // first match
playwright.$$('li')                  // all matches
playwright.inspect('button')         // reveal in Elements panel
playwright.locator('button').click() // test a locator
```

### 3.4 Trace Viewer (post-mortem debugging)

Tracing records everything — screenshots, DOM snapshots, network requests, console output, and which line of your code triggered each action — so you can debug a failure **after** it happens, including headless and CI runs.

```python
context = await browser.new_context()
await context.tracing.start(screenshots=True, snapshots=True, sources=True)

try:
    page = await context.new_page()
    await page.goto("https://example.com")
    # ... your steps ...
finally:
    await context.tracing.stop(path="trace.zip")
```

Open it:

```bash
playwright show-trace trace.zip
```

In the viewer you can click through each action on a timeline and see the page exactly as it was before and after, plus the network log and console messages at that moment.

**Tip:** to keep traces only when something goes wrong, stop tracing without a path on success:

```python
try:
    ...
    await context.tracing.stop()            # discard
except Exception:
    await context.tracing.stop(path="trace.zip")  # keep
    raise
```

### 3.5 Python debugger (breakpoints)

Normal Python breakpoints work in async code:

```python
await page.goto(url)
breakpoint()  # drops into pdb here
```

At the `pdb` prompt you can't use `await` directly. To inspect page state, prefer `page.pause()` or use VS Code's debugger, which handles async code well.

**VS Code** — `.vscode/launch.json`:

```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Playwright script",
      "type": "debugpy",
      "request": "launch",
      "program": "${file}",
      "console": "integratedTerminal",
      "justMyCode": false,
      "env": { "PWDEBUG": "1" }
    }
  ]
}
```

Remove the `PWDEBUG` line if you only want Python breakpoints without the Inspector.

### 3.6 Verbose API logs

See every Playwright call and what it's waiting on — useful for "why is this hanging?":

```bash
DEBUG=pw:api python script.py
```

### 3.7 Capture page output

Pipe the page's own console messages and errors into your terminal:

```python
page.on("console", lambda msg: print(f"[console] {msg.type}: {msg.text}"))
page.on("pageerror", lambda err: print(f"[pageerror] {err}"))
```

### 3.8 Generate code with Codegen

Let Playwright write the first draft of a script by recording you:

```bash
playwright codegen https://example.com
```

Choose **Python Async** as the target in the Codegen window, click around, then copy the generated code.

---

## 4. Which Tool When

| Situation | Use |
|---|---|
| Writing a new script | `playwright codegen` |
| A locator doesn't match / matches the wrong thing | Inspector → **Pick locator** or the locator field |
| Want to step through actions one by one | `PWDEBUG=1` |
| Need to stop at one specific spot | `await page.pause()` |
| Script fails headless, in cron, or in CI | Trace Viewer |
| Script hangs with no error | `DEBUG=pw:api` |
| Bug is in your Python logic, not the page | VS Code debugger / `breakpoint()` |