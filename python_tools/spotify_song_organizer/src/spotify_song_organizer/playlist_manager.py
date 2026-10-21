import requests


def get_my_playlists(session: requests.Session, user_headers):
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


def get_or_create_playlist(name, playlists, session: requests.Session):
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


def add_many(playlist_id, uris, session: requests.Session):
    def add_to_playlist(playlist_id, uris):
        r = session.post(
            f"https://api.spotify.com/v1/playlists/{playlist_id}/items",
            json={"uris": uris},
        )
        if r.status_code != 201:
            print(r.text)
        r.raise_for_status()
        return r.json()["snapshot_id"]

    for i in range(0, len(uris), 100):
        add_to_playlist(playlist_id, uris[i : i + 100])


def playlist_uris(playlist_id, session: requests.Session):
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


def get_playlist_tracks(playlist_id, session: requests.Session):
    tracks = []
    url = f"https://api.spotify.com/v1/playlists/{playlist_id}/items?limit=50"
    while url:
        r = session.get(url)
        if r.status_code != 200:
            print(r.text)
        r.raise_for_status()
        page = r.json()
        for entry in page["items"]:
            track = entry.get("item")  # "item", not "track", for playlists
            if track and track.get("type") == "track" and not track.get("is_local"):
                tracks.append(track)
        url = page["next"]
    return tracks
