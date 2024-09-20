import time
from google_apis import create_service


CLIENT_FILE = 'gmail_delete_service_secrets.json'
API_NAME='gmail'
API_VERSION='v1'
SCOPES=['https://mail.google.com/']

gmail_service = create_service(CLIENT_FILE,API_NAME,API_VERSION,SCOPES)


def search_emails(query, labels=None):
    email_messages = []
    next_page_token = None
    
    message_response = gmail_service.users().messages().list(
        userId='me',
        labelIds=labels,
        includeSpamTrash=False,
        q=query,
        maxResults=500
    ).execute()
    email_messages = message_response.get('messages')
    next_page_token = message_response.get('nextPageToken')

    while next_page_token:
        message_response = gmail_service.users().messages().list(
            userId='me',
            labelIds=labels,
            q=query,
            maxResults=500,
            includeSpamTrash=False,
            pageToken=next_page_token
        ).execute()
        email_messages.extend(message_response['messages'])
        next_page_token = message_response.get('nextPageToken')
        print('Page Token: {0}'.format(next_page_token))
        time.sleep(0.5)
    return email_messages
SEARCH_QUERY = 'from: (DestinyCard OR milestonecard)'
email_results = search_emails(query=SEARCH_QUERY,)
print(len(email_results))