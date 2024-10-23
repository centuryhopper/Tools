namespace LeoPasswordManager.Models;

public class PasswordAccountModel
{
    public int Id { get; set; }
    public int UserId { get; set; }
    public string? Title { get; set; }
    public string? Username { get; set; }
    public string Password { get; set; }
    public DateTime? CreatedAt { get; set; }
    public DateTime? LastUpdatedAt { get; set; }

    public override string ToString()
    {
        return $"{nameof(Id)}:{Id}, {nameof(Title)}:{Title}, {nameof(Username)}:{Username}, {nameof(Password)}:{Password}, {nameof(UserId)}:{UserId}";
    }
}