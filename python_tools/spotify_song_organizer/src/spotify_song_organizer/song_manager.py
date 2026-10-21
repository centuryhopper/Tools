def unlike(uris, session):
    for i in range(0, len(uris), 40):  # max 40 URIs per request
        r = session.delete(
            "https://api.spotify.com/v1/me/library",
            params={"uris": ",".join(uris[i : i + 40])},
        )
        if r.status_code != 200:
            print(r.text)
        r.raise_for_status()
