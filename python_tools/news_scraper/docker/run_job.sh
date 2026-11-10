#!/bin/sh
. /app/cron.env
cd /app
exec /app/.venv/bin/python news_scraper.py
