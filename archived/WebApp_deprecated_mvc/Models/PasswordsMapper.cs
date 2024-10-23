using CsvHelper.Configuration;
using LeoPasswordManager.Entities;

namespace LeoPasswordManager.Models;

public class PasswordsMapper : ClassMap<PasswordmanagerAccount>
{
    public PasswordsMapper()
    {
        Map(m => m.Title).Name("Title");
        Map(m => m.Username).Name("Username");
        Map(m => m.Password).Name("Password");
    }
}