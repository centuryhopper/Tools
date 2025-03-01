
namespace dotnet_bmbl_bot.Models;

public class User
{
    public string UserName { get; set; }
    public string Password { get; set; }
    public string Name { get; set; }

    public override string ToString()
    {
        return $"{nameof(UserName)}:{UserName},{nameof(Password)}:{Password},{nameof(Name)}:{Name}";
    }
}