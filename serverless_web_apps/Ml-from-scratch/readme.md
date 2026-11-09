# ML From Scratch (JavaScript)

Machine learning algorithms written from scratch in vanilla JavaScript and visualized in HTML. Starting with **K-Nearest Neighbors (KNN)** on the **Breast Cancer Wisconsin (Diagnostic)** dataset from Kaggle.

No frameworks, no build step, no npm — just HTML, JS, and a CSV.

## Running locally

The page has to be served over HTTP. Opening `index.html` directly (`file://...`) won't work, because browsers block `fetch()` of the CSV and loading ES modules (`<script type="module">`) from the local filesystem.

Python's built-in web server is enough:

```bash
cd path/to/this/project
python3 -m http.server 8000
```

Then open <http://localhost:8000> in your browser.

- Stop the server with **Ctrl-C**.
- Port already in use? Pick another one: `python3 -m http.server 8000`.
- Only want it reachable from your own machine? Add `--bind 127.0.0.1`.

### Workflow

1. Start the server in a terminal (or a tmux pane) and leave it running.
2. Edit the JS/HTML in your editor.
3. Refresh the browser to see changes. If you see stale code, do a hard refresh (**Ctrl-Shift-R**), since the browser may have cached it.
4. Use the browser dev tools console (**F12**) for `console.log` output and errors.

## Dataset

Download the Breast Cancer Wisconsin (Diagnostic) dataset from Kaggle and put the CSV in the project folder (e.g. `data/data.csv`). Fetch it with a path **relative to the server root**:

```js
const res = await fetch("data/data.csv");
const text = await res.text();
```

## Deploying to GitHub Pages

The site is plain static files, so it deploys as-is:

1. Push the repo to GitHub.
2. Go to **Settings → Pages**.
3. Under **Source**, choose **Deploy from a branch**, then pick `main` and `/ (root)`.
4. The site will be live at `https://<username>.github.io/<repo>/`.

Keep all paths relative (`data/data.csv`, not `/data/data.csv`). On GitHub Pages the site lives under `/<repo>/`, so paths starting with `/` would break.

## Roadmap

- [ ] KNN
- [ ] More algorithms
