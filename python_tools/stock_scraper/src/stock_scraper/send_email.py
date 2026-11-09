import os
import sys

import resend
from dotenv import load_dotenv

load_dotenv()

resend.api_key = os.environ.get("RESEND_API_KEY")
if not resend.api_key:
    sys.exit("RESEND_API_KEY is not set")


def send_email(to: str | list[str], subject: str, html: str, text: str | None = None) -> str:
    """Send an email through Resend and return its email ID."""
    params: resend.Emails.SendParams = {
        # Must be an address on a domain you've verified in Resend.
        # For testing, "onboarding@resend.dev" works, but only sends to your own account email.
        "from": os.environ.get("RESEND_FROM", ""),
        "to": [to] if isinstance(to, str) else to,
        "subject": subject,
        "html": html,
    }
    if text:
        params["text"] = text  # plain-text fallback improves deliverability

    email = resend.Emails.send(params)
    return email["id"]


# if __name__ == "__main__":
#     recipient = sys.argv[1] if len(sys.argv) > 1 else "you@example.com"
#     try:
#         email_id = send_email(
#             to=recipient,
#             subject="Hello from Resend",
#             html="<h1>It works!</h1><p>This email was sent with the Resend Python SDK.</p>",
#             text="It works! This email was sent with the Resend Python SDK.",
#         )
#         print(f"Sent: {email_id}")
#     except Exception as e:
#         sys.exit(f"Failed to send: {e}")
