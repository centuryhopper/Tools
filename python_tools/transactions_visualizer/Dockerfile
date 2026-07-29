FROM python:3.12-slim

WORKDIR /app

COPY --from=ghcr.io/astral-sh/uv:latest /uv /usr/local/bin/uv

COPY pyproject.toml uv.lock README.md ./
COPY src ./src

RUN uv sync --frozen

COPY . .

CMD ["sh", "-c", "uv run streamlit run transactions_processor_pipeline.py --server.port=${PORT:-8501} --server.address=0.0.0.0"]
