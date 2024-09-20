using System;
using System.Collections.Generic;

namespace Portfolio.Contexts;

public partial class Blog
{
    public int Id { get; set; }

    public string Title { get; set; } = null!;

    public DateTime Date { get; set; }

    public string PreviewDesc { get; set; } = null!;

    public string? RouteName { get; set; }

    public string FullDesc { get; set; } = null!;

    public virtual ICollection<VideoUrl> VideoUrls { get; set; } = new List<VideoUrl>();
}
