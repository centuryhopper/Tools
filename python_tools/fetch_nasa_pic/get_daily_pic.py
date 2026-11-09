import os
from datetime import datetime
from pathlib import Path
from urllib.parse import urlparse

import requests
from dotenv import load_dotenv, find_dotenv

script_dir = os.path.dirname(os.path.abspath(__file__))

# find_dotenv will start at script_dir and walk UP until it finds secrets.env
load_dotenv(find_dotenv(os.path.join(script_dir, "secrets.env")))

# Where pictures are saved. In Docker this is the mounted volume (/data);
# locally it defaults to ./nasa_pic_collection next to this script.
PIC_DIR = Path(os.getenv("NASA_PIC_DIR", Path(__file__).parent / "nasa_pic_collection"))
# Get a free key at https://api.nasa.gov — DEMO_KEY is heavily rate-limited.
API_KEY = os.getenv("NASA_API_KEY", "DEMO_KEY")
APOD_URL = "https://api.nasa.gov/planetary/apod"
TIMEOUT = 60


def download_pic_of_the_day() -> None:
    today_string = datetime.now().strftime("%Y-%m-%d")
    today_folder = PIC_DIR / today_string
    info_file = today_folder / f"{today_string}.txt"

    # Only skip if a previous run actually finished (the .txt is written last).
    if info_file.exists():
        print(f"[{datetime.now():%H:%M}] already gathered today's image")
        return

    response = requests.get(APOD_URL, params={"api_key": API_KEY}, timeout=TIMEOUT)
    response.raise_for_status()
    if not response.ok:
        print(f"NASA API returned {response.status_code}: {response.text[:300]}")
        raise SystemExit(1)
    data = response.json()

    fields = {
        "Date": data.get("date", "n/a"),
        "Explanation": data.get("explanation", "n/a"),
        "Hdurl": data.get("hdurl", "n/a"),
        "Media_type": data.get("media_type", "n/a"),
        "Service_version": data.get("service_version", "n/a"),
        "Title": data.get("title", "n/a"),
        "Url": data.get("url", "n/a"),
        # sometimes there's no copyright available
        "Copyright": data.get("copyright", "n/a"),
    }

    today_folder.mkdir(parents=True, exist_ok=True)

    # Some days APOD is a video, which has no hdurl — then only the .txt is saved.
    image_url = data.get("hdurl") or (
        data.get("url") if data.get("media_type") == "image" else None
    )
    if image_url:
        image_response = requests.get(image_url, timeout=TIMEOUT)
        image_response.raise_for_status()
        ext = Path(urlparse(image_url).path).suffix or ".jpg"
        (today_folder / f"nasa_pic_{today_string}{ext}").write_bytes(
            image_response.content
        )
    else:
        print(f"today's APOD is a {fields['Media_type']}, saving info only")

    info_file.write_text(
        "\n\n".join(f"{key}: {value}" for key, value in fields.items()),
        encoding="utf-8",
    )
    print(f"[{datetime.now():%H:%M}] saved {today_string}: {fields['Title']}")


if __name__ == "__main__":
    PIC_DIR.mkdir(parents=True, exist_ok=True)
    download_pic_of_the_day()
