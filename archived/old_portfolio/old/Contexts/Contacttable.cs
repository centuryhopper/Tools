using System;
using System.Collections.Generic;

namespace Portfolio.Contexts;

public partial class Contacttable
{
    public int Id { get; set; }

    public string Name { get; set; } = null!;

    public string Email { get; set; } = null!;

    public string Subject { get; set; } = null!;

    public string Message { get; set; } = null!;
}
