import base64
import collections
import os
import sys
import webbrowser
from concurrent.futures import ThreadPoolExecutor
from pprint import pprint
from typing import Dict, List, Set
from urllib.parse import urlencode

import requests
from dotenv import load_dotenv

from spotify_song_organizer.genres import genres_list
from spotify_song_organizer.playlist_manager import *
from spotify_song_organizer.token_manager import get_access_token, save_tokens

load_dotenv("secrets.env")

import json
import time

TOKEN_FILE = "secrets.json"

client_id = os.getenv("SPOTIFY_CLIENT_ID")
client_secret = os.getenv("SPOTIFY_CLIENT_SECRET")

playlist_id_to_set_dict: Dict[str, Set[str]] = {}

playlist_id_to_uri_array_dict: Dict[str, List[str]] = collections.defaultdict(list)

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
        "scope": "user-library-read playlist-read-private playlist-read-collaborative user-library-modify "
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


def put_song_in_playlist(uri: str, playlist_name: str = "Genre: UNKNOWN"):
    potentially_new_playlist = get_or_create_playlist(
        playlist_name, playlists=playlists, session=session
    )
    if potentially_new_playlist not in playlist_id_to_set_dict:
        existingSongs = playlist_uris(potentially_new_playlist, session=session)
        playlist_id_to_set_dict[potentially_new_playlist] = existingSongs
    if uri not in playlist_id_to_set_dict[potentially_new_playlist]:
        playlist_id_to_uri_array_dict[potentially_new_playlist].append(uri)


def fetch_tags(track):
    """Runs in worker threads. Returns (uri, lowercase tag names), or (uri, None) on failure."""
    song_name, artist, uri = track
    for attempt in range(3):
        try:
            r = lastfm_session.get(
                "https://ws.audioscrobbler.com/2.0/",
                params={
                    "method": "track.gettoptags",
                    "artist": artist,
                    "track": song_name,
                    "api_key": os.getenv("LASTFM_API_KEY"),
                    "format": "json",
                    "autocorrect": 1,
                },
                timeout=15,
            )
            data = r.json()
        except (requests.RequestException, ValueError):
            time.sleep(2**attempt)
            continue
        if data.get("error") == 29:  # Last.fm: rate limit exceeded
            time.sleep(2**attempt)
            continue
        tags = data.get("toptags", {}).get("tag", [])
        return uri, [t["name"].lower() for t in tags]
    return uri, None  # all retries failed


if __name__ == "__main__":
    if not os.path.exists(TOKEN_FILE):
        login()

    token = get_access_token()
    user_headers = {"Authorization": "Bearer " + token, "Content-Type": "application/json"}
    session = requests.Session()
    session.headers.update(user_headers)
    lastfm_session = requests.Session()

    playlists = get_my_playlists(session, user_headers)  # fetch once

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
        (
            track["track"]["name"],
            track["track"]["artists"][0]["name"],
            track["track"]["uri"],
        )
        for track in all_items
    ]
    # Phase 1: parallel, network only
    with ThreadPoolExecutor(max_workers=5) as pool:
        results = list(pool.map(fetch_tags, tracks))

    # Phase 2: sequential, your existing playlist logic
    # group each song by their genre
    # if genre list from request is empty then put into "genre: unknown" playlist
    for uri, tags in results:
        if tags is None:
            continue  # lookup failed; skip so it isn't wrongly filed as UNKNOWN
        genre: str | None = next((t for t in tags if t in genres_list), None)
        # genre would really ever be None if none of the tags were found in the list
        if genre is None:
            put_song_in_playlist(uri)
        else:
            # create playlist with the name of "genre: o['name'].upper()" (if it doesnt exist already)
            # and add the song to that playlist
            put_song_in_playlist(uri, f"Genre: {genre.upper()}")

        # TODO: remove this song from liked songs


    # populate genre playlists
    for playlist_id, lstOfUris in playlist_id_to_uri_array_dict.items():
        add_many(playlist_id, lstOfUris, session=session)

    end = time.perf_counter()
    print("total time: {} seconds".format(end - start))
