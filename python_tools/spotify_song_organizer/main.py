import base64
import os
import webbrowser
from pprint import pprint
from urllib.parse import urlencode

import requests
from dotenv import load_dotenv

from spotify_song_organizer.genres import genres_list
from spotify_song_organizer.token_management import get_access_token, save_tokens

load_dotenv("secrets.env")

import json
import time

TOKEN_FILE = "secrets.json"

client_id = os.getenv("SPOTIFY_CLIENT_ID")
client_secret = os.getenv("SPOTIFY_CLIENT_SECRET")


import time

"""
- connect to your spotify account
- get every playlist in your own library
- one playlist at a time, loop thru each song in it, get the genre of each song and add it to the playlist named after the genre (or create it if it doesn't exist already)
"""


def login():
    auth_headers = {
        "client_id": client_id,
        "response_type": "code",
        "redirect_uri": "http://127.0.0.1:8888/callback",
        "scope": "user-library-read playlist-read-private playlist-read-collaborative "
        "playlist-modify-private playlist-modify-public",
    }
    webbrowser.open("https://accounts.spotify.com/authorize?" + urlencode(auth_headers))

    encoded_credentials = base64.b64encode(
        client_id.encode() + b":" + client_secret.encode()
    ).decode("utf-8")

    token_headers = {
        "Authorization": "Basic " + encoded_credentials,
        "Content-Type": "application/x-www-form-urlencoded",
    }

    code = input("Paste the code: ")

    token_data = {
        "grant_type": "authorization_code",
        "code": code,
        "redirect_uri": "http://127.0.0.1:8888/callback",
    }

    r = requests.post(
        "https://accounts.spotify.com/api/token", data=token_data, headers=token_headers
    )

    # pprint(r.json())

    return save_tokens(r.json())


if not os.path.exists(TOKEN_FILE):
    login()

token = get_access_token()


user_headers = {"Authorization": "Bearer " + token, "Content-Type": "application/json"}
session = requests.Session()
session.headers.update(user_headers)

url = "https://api.spotify.com/v1/me/tracks?limit=50"

all_items = []

start = time.perf_counter()
while url:
    r = session.get(url, headers=user_headers)
    r.raise_for_status()
    page = r.json()
    all_items.extend(page["items"])
    url = page["next"]

all_items.sort(key=lambda t: t["added_at"])  # oldest first, across all pages
tracks = [
    (track["track"]["name"], track["track"]["artists"][0]["name"])
    for track in all_items
]
pprint(tracks)
print(len(tracks))
# pprint(all_items)
end = time.perf_counter()

# https://ws.audioscrobbler.com/2.0/?method=track.gettoptags&artist=lakey inspired&track=goodnight&api_key=SECRET&format=json&autocorrect=1

# pprint(genres_list)

# group each song by their genre
# if genre list from request is empty then group by the artist

print("total time: {} seconds".format(end - start))

