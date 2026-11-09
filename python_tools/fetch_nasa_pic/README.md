# fetch_nasa_pic

Downloads NASA's Astronomy Picture of the Day at 9am, 12pm, 3pm, 6pm and 9pm.
Each day gets its own folder with the image and a `.txt` of its details.

## First-time setup (on any machine with uv)

```sh
uv lock          # creates uv.lock — commit it
uv run get_daily_pic.py   # optional local test
```

## Run on the Pi 5

```sh
cp .env.example .env      # add your NASA API key and picture folder
docker compose up -d --build
docker compose logs -f    # see fetches as they happen
```

Run a fetch now: `docker compose exec fetch_nasa_pic /app/docker/run_job.sh`

After changing dependencies (`uv add ...`), rebuild with `docker compose up -d --build`.
