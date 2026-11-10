# news_scraper

Saves US top headlines from [NewsAPI](https://newsapi.org) for six categories
(business, entertainment, health, science, sports, technology) into one Excel
file per day, one sheet per category. Runs at 9am, 12pm, 3pm, 6pm and 9pm
Eastern; the first successful run of the day saves the file, later runs skip.

## First-time setup (on any machine with uv)

```sh
uv lock                     # creates uv.lock — commit it
cp secrets.env.example secrets.env   # add your NewsAPI key
uv run news_scraper.py      # optional local test, saves to ./news/
```

## Run on the Pi 5

```sh
cp secrets.env.example secrets.env   # if not copied over already
docker compose up -d --build
docker compose logs -f
```

Excel files land in `/home/leo_zhang/files_from_docker/news` (change the path in
`compose.yaml` if needed).

Run a fetch now: `docker exec news_scraper /app/docker/run_job.sh`

After changing dependencies (`uv add ...`), rebuild with `docker compose up -d --build`.
