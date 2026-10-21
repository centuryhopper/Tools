import json
import time
import os
from dotenv import load_dotenv

load_dotenv("secrets.env")

import requests


TOKEN_FILE = "secrets.json"
TOKEN_URL = "https://accounts.spotify.com/api/token"

client_id = os.getenv("SPOTIFY_CLIENT_ID")
client_secret = os.getenv("SPOTIFY_CLIENT_SECRET")

def save_tokens(data, old_refresh_token=None):
    tokens = {
        "access_token": data["access_token"],
        # Spotify doesn't always return a new refresh token; keep the old one if not
        "refresh_token": data.get("refresh_token", old_refresh_token),
        "expires_at": time.time() + data["expires_in"],
    }
    with open(TOKEN_FILE, "w") as f:
        json.dump(tokens, f)
    return tokens


def refresh_tokens(refresh_token):
    r = requests.post(
        TOKEN_URL,
        data={"grant_type": "refresh_token", "refresh_token": refresh_token},
        auth=(client_id, client_secret),
    )
    r.raise_for_status()
    return save_tokens(r.json(), old_refresh_token=refresh_token)


def get_access_token():
    with open(TOKEN_FILE) as f:
        tokens = json.load(f)

    # refresh a minute early so a token doesn't expire mid-request
    if time.time() > tokens["expires_at"] - 60:
        tokens = refresh_tokens(tokens["refresh_token"])

    return tokens["access_token"]
