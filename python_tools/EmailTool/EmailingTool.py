import collections
from typing import List
import smtplib, ssl
from email.message import EmailMessage
from dotenv import load_dotenv
import os

load_dotenv()

# subtypes can be found here: https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
# Working in 2023:
# https://www.youtube.com/watch?v=g_j6ILT-X0k&ab_channel=ThePyCoach

class EmailTool:
    outputFilePath = ''
    senderEmail = os.getenv('senderEmail')
    senderEmailPassword = os.getenv('senderEmailPassword')
    receiverEmails = []
    # EmailCredentials = collections.namedtuple("EmailCredentials", ['password', 'sender', 'recipients'])

    @staticmethod
    def sendEmail(filePathComplete: str='', fileName: str='', recipients:List[str]=[], subject:str='', msg:str='', subtype=''):
        port = 465  # For SSL
        smtp_server = "smtp.gmail.com"
        context = ssl.create_default_context()
        with smtplib.SMTP_SSL(smtp_server, port, context=context) as server:
            try:
                server.login(EmailTool.senderEmail, EmailTool.senderEmailPassword)
                for recipient in recipients:
                    # must create a new EmailMessage object for every recipient
                    message = EmailMessage()
                    message['From'] = EmailTool.senderEmail
                    message['To'] = recipient
                    message['Subject'] = subject
                    message.set_content(msg)
                    if filePathComplete and fileName:
                        with open(filePathComplete, 'rb') as f:
                            fileData = f.read()
                        message.add_attachment(fileData, maintype="application", subtype=subtype, filename=fileName)
                    server.send_message(message)
                print('email sent!')
            except Exception as e:
                print(e)
                print("could not login or send the mail.")


# EmailTool.sendEmail('','', ['leozhang12345678@gmail.com'], 'test', 'yo there')
