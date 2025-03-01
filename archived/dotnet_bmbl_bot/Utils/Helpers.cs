using MimeKit;
using MailKit.Net.Smtp;

namespace dotnet_bmbl_bot.Utils;

public static class Helpers
{
    public static void SendEmail(
        string subject,
        string body,
        string sender,
        string senderPassword,
        IEnumerable<string> receivers
    )
    {
        var msg = new MimeMessage();

        msg.From.Add(new MailboxAddress("Automated Email Message", sender));

        foreach (var receiver in receivers)
        {
            msg.To.Add(MailboxAddress.Parse(receiver));
        }

        msg.Subject = subject;

        msg.Body = new TextPart("plain") { Text = body, };

        var client = new SmtpClient();

        try
        {
            client.Connect("smtp.gmail.com", 465, true);
            client.Authenticate(sender, senderPassword);
            var res = client.Send(msg);
        }
        catch (System.Exception ex)
        {
            System.Console.WriteLine(ex.Message);
        }
        finally
        {
            client.Disconnect(true);
            client.Dispose();
        }
    }



}
