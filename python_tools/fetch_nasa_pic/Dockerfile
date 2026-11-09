# syntax=docker/dockerfile:1
# Official uv image with Python 3.13 — multi-arch, so it builds natively on a Pi 5 (arm64).
FROM ghcr.io/astral-sh/uv:python3.13-bookworm-slim

ARG TZ=America/New_York
ENV TZ=${TZ} \
    PYTHONUNBUFFERED=1 \
    UV_COMPILE_BYTECODE=1 \
    UV_LINK_MODE=copy \
    UV_PYTHON_DOWNLOADS=never \
    NASA_PIC_DIR=/data

RUN apt-get update \
 && apt-get install -y --no-install-recommends cron tzdata \
 && ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo "$TZ" > /etc/timezone \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Dependencies first so this layer stays cached until uv.lock changes.
COPY pyproject.toml uv.lock .python-version ./
RUN --mount=type=cache,target=/root/.cache/uv \
    uv sync --locked --no-dev --no-install-project

COPY get_daily_pic.py ./
COPY docker/ ./docker/
# Strip Windows line endings just in case, then register the cron schedule.
RUN sed -i 's/\r$//' docker/* \
 && chmod +x docker/*.sh \
 && crontab docker/crontab

VOLUME ["/data"]
ENTRYPOINT ["/app/docker/entrypoint.sh"]
