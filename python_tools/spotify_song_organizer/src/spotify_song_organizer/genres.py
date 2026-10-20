from pathlib import Path
from pprint import pprint

BASE_DIR = Path(__file__).resolve().parent

# genre list gathered from: https://musicbrainz.org/ws/2/genre/all?fmt=txt
genres_list = []
with open(BASE_DIR / "genres.txt", "r") as f:
    genres_list = [line.strip() for line in f.readlines()]

# pprint(genres)
