#!/bin/sh
. /app/cron.env
cd /app
exec /app/.venv/bin/python get_daily_pic.py
