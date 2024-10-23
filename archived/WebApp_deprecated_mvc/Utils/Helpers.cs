using Microsoft.AspNetCore.Mvc.ModelBinding;
using MimeKit;
using MailKit.Net.Smtp;
using LeoPasswordManager.Models;
using LeoPasswordManager.Contexts;
using LeoPasswordManager.Entities;

namespace LeoPasswordManager.Utilities;

public static class Helpers
{
    public static IEnumerable<string> GetErrors<T>(this ModelStateDictionary ModelState, ILogger<T>? logger = null)
    {
        // Retrieve the list of errors
        var errors = ModelState.Values.SelectMany(v => v.Errors);
        if (logger is not null)
        {
            errors.ToList().ForEach(e => logger.LogWarning(e.ErrorMessage));
        }

        return errors.Select(e => e.ErrorMessage);
    }

    public static void SendEmail(
        string subject,
        string body,
        string senderEmail,
        string senderPassword,
        IEnumerable<string> receivers
    )
    {
        var msg = new MimeMessage();

        msg.From.Add(new MailboxAddress("Automated Email Message", senderEmail));

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
            client.Authenticate(senderEmail, senderPassword);
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

    public static PasswordManagerUserVM ToPasswordManagerUserVM(this PasswordmanagerUser dto)
    {
        return new()
        {
            Id = dto.Id,
            Email = dto.Email,
            Firstname = dto.Firstname,
            Lastname = dto.Lastname,
            Datelastlogin = dto.Datelastlogin,
            Datelastlogout = dto.Datelastlogout,
            Datecreated = dto.Datecreated,
            Dateretired = dto.Dateretired,
        };
    }

    public static PasswordmanagerUser ToPasswordManagerUser(this PasswordManagerUserVM dto)
    {
        return new()
        {
            Id = dto.Id,
            Email = dto.Email,
            Firstname = dto.Firstname,
            Lastname = dto.Lastname,
            Datelastlogin = dto.Datelastlogin,
            Datelastlogout = dto.Datelastlogout,
            Datecreated = dto.Datecreated,
            Dateretired = dto.Dateretired,
        };
    }


    public static PasswordmanagerAccount ToPasswordManagerAccount(this PasswordAccountModel dto)
    {
        return new()
        {
            // ignore assigning id because id here is an autogenerating primary key
            // Id = dto.Id!
            // ,
            Userid = dto.UserId!
            ,
            Title = dto.Title!
            ,
            Username = dto.Username!
            ,
            Password = dto.Password!
            ,
            CreatedAt = dto.CreatedAt
            ,
            LastUpdatedAt = dto.LastUpdatedAt
        };
    }

    public static PasswordAccountModel ToPasswordManagerAccountModel(this PasswordmanagerAccount account)
    {
        return new()
        {
            Id = account.Id
            ,
            UserId = account.Userid
            ,
            Title = account.Title
            ,
            Username = account.Username
            ,
            Password = account.Password
            ,
            CreatedAt = account.CreatedAt
            ,
            LastUpdatedAt = account.LastUpdatedAt
        };
    }

}
