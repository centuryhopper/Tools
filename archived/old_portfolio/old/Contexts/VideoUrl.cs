using System;
using System.Collections.Generic;

namespace Portfolio.Contexts;

public partial class VideoUrl
{
    public int Id { get; set; }

    public int BlogId { get; set; }

    public string Url { get; set; } = null!;

    public virtual Blog Blog { get; set; } = null!;
}
