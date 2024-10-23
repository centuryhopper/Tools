using System;
using System.Collections.Generic;

namespace LeoPasswordManager.Entities;

public partial class PasswordmanagerAccount
{
    public int Id { get; set; }

    public int Userid { get; set; }

    public string Title { get; set; } = null!;

    public string Username { get; set; } = null!;

    public string Password { get; set; } = null!;

    public DateTime? CreatedAt { get; set; }

    public DateTime? LastUpdatedAt { get; set; }

    public virtual PasswordmanagerUser User { get; set; } = null!;
}
