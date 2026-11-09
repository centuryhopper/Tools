#!/bin/sh
set -e

# cron starts jobs with an empty environment, so save the variables the job
# needs where run_job.sh can load them.
export -p | grep -E "^export (NASA_[A-Z_]*|TZ)=" > /app/cron.env
chmod 600 /app/cron.env

# Fetch once at startup (the script skips if today's picture already exists).
/app/docker/run_job.sh || echo "startup fetch failed; cron will retry later"

echo "cron started — schedule: $(grep -v '^#' /app/docker/crontab | cut -d' ' -f1-5)"
exec cron -f
