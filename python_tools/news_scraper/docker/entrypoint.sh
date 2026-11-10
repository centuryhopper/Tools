#!/bin/sh
set -e

# cron starts jobs with an empty environment, so save the container's
# variables (API key, TZ, NEWS_DIR, ...) where run_job.sh can load them.
export -p > /app/cron.env
chmod 600 /app/cron.env

# Fetch once at startup (the script skips if today's file already exists).
/app/docker/run_job.sh || echo "startup fetch failed; cron will retry later"

echo "cron started — schedule: $(grep -v '^#' /app/docker/crontab | cut -d' ' -f1-5)"
exec cron -f
