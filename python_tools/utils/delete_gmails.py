import time
from google_apis import create_service
import pprint
import os

os.chdir(os.path.dirname(__file__))


CLIENT_FILE = "gmail_delete_service_secrets.json"
API_NAME = "gmail"
API_VERSION = "v1"
SCOPES = ["https://mail.google.com/"]
gmail_service = create_service(CLIENT_FILE, API_NAME, API_VERSION, SCOPES)


def search_emails(query, labels=None):
    email_messages = []
    next_page_token = None

    message_response = (
        gmail_service.users()
        .messages()
        .list(
            userId="me",
            labelIds=labels,
            includeSpamTrash=False,
            q=query,
            maxResults=500,
        )
        .execute()
    )
    email_messages = message_response.get("messages")
    next_page_token = message_response.get("nextPageToken")

    while next_page_token:
        message_response = (
            gmail_service.users()
            .messages()
            .list(
                userId="me",
                labelIds=labels,
                q=query,
                maxResults=500,
                includeSpamTrash=False,
                pageToken=next_page_token,
            )
            .execute()
        )
        email_messages.extend(message_response["messages"])
        next_page_token = message_response.get("nextPageToken")
        print("Page Token: {0}".format(next_page_token))
        time.sleep(0.5)
    return email_messages


SEARCH_QUERY = '''from: (Destinycard OR milestone OR "automated email message" OR mastercard)'''
email_results = search_emails(
    query=SEARCH_QUERY,
)
# pprint.pprint(email_results)

# for email_result in email_results:
#     email_detail = gmail_service.users().messages().get(
#         userId='me',
#         format='full',
#         id=email_result['id']
#     ).execute()
#     pprint.pprint(email_detail)

for email_result in [] if not email_results else email_results:
    gmail_service.users().messages().trash(userId="me", id=email_result["id"]).execute()

print(
    "number of emails deleted: {}".format(
        len([] if not email_results else email_results)
    )
)
