To view your transactions in the cli:

`curl -X POST https://production.plaid.com/transactions/get \
  -H "Content-Type: application/json" \
  -d '{
    "client_id": "YOUR_CLIENT_ID",
    "secret": "YOUR_PRODUCTION_SECRET",
    "access_token": "YOUR_PRODUCTION_ACCESS_TOKEN",
    "start_date": "2024-06-01",
    "end_date": "2025-06-17",
    "options": {
      "count": 100,
      "offset": 0
    }
  }'`
