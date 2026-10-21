import base64
import os
import sys
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


def get_my_playlists():
    """Return {name: id} for playlists you own."""
    me = session.get("https://api.spotify.com/v1/me", headers=user_headers).json()
    me = me["id"]

    playlists = {}
    url = "https://api.spotify.com/v1/me/playlists?limit=50"
    while url:
        r = session.get(url, headers=user_headers)
        r.raise_for_status()
        page = r.json()
        for p in page["items"]:
            if p["owner"]["id"] == me:  # skip playlists you only follow
                playlists[p["name"]] = p["id"]
        url = page["next"]
    return playlists


def get_or_create_playlist(name, playlists):
    if name in playlists:
        return playlists[name]

    r = session.post(
        "https://api.spotify.com/v1/me/playlists",
        json={
            "name": name,
            "public": True,
            "description": "Created by Spotify Song Organizer",
        },
    )
    r.raise_for_status()
    playlist_id = r.json()["id"]
    playlists[name] = playlist_id  # remember it so later calls don't create it again
    return playlist_id


def add_to_playlist(playlist_id, uris):
    r = session.post(
        f"https://api.spotify.com/v1/playlists/{playlist_id}/items",
        json={"uris": uris},
    )
    if r.status_code != 201:
        print(r.text)
    r.raise_for_status()
    return r.json()["snapshot_id"]


def add_many(playlist_id, uris):
    for i in range(0, len(uris), 100):
        add_to_playlist(playlist_id, uris[i : i + 100])


def playlist_uris(playlist_id):
    uris = set()
    url = f"https://api.spotify.com/v1/playlists/{playlist_id}/items?limit=50"
    while url:
        r = session.get(url)
        r.raise_for_status()
        page = r.json()
        for entry in page["items"]:
            if entry.get("item"):
                uris.add(entry["item"]["uri"])
        url = page["next"]
    return uris


# existing = playlist_uris(rock_id)
# new_uris = [u for u in rock_uris if u not in existing]
# add_many(rock_id, new_uris)


playlists = get_my_playlists()  # fetch once

sys.exit(1)

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
# pprint(tracks)
# print(len(tracks))
# pprint(all_items)
end = time.perf_counter()

# https://ws.audioscrobbler.com/2.0/?method=track.gettoptags&artist=lakey inspired&track=goodnight&api_key=SECRET&format=json&autocorrect=1


# pprint(genres_list)
lastfm_session = requests.Session()
for song_name, artist in tracks:
    r = lastfm_session.get(
        f"https://ws.audioscrobbler.com/2.0/?method=track.gettoptags&artist={artist}&track={song_name}&api_key={os.getenv('57673e262de15381fd45b9ecc555058d')}&format=json&autocorrect=1"
    )
    output = r.json()
    tags = output["toptags"]["tag"]
    if not tags:
        # put song in "genre: unknown" playlist
        pass
    else:
        didAddToPlaylist = False
        for o in tags:
            if o["name"] in genres_list:
                # create playlist with the name of "genre: o['name'].upper()" (if it doesnt exist already)
                # and add the song to that playlist
                # remove this song from liked songs
                didAddToPlaylist = True
                new_playlist = get_or_create_playlist(
                    f"Genre: {o['name'].upper()}", playlists=playlists
                )
                break
        if not didAddToPlaylist:
            # put song in "genre: unknown" playlist
            new_playlist = get_or_create_playlist(
                f"Genre: UNKNOWN", playlists=playlists
            )
            pass


# group each song by their genre
# if genre list from request is empty then put into "genre: unknown" playlist

print("total time: {} seconds".format(end - start))
