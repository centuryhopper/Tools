using System;
using System.Collections.Generic;

namespace Portfolio.Contexts;

public partial class ProjectCard
{
    public int Id { get; set; }

    public string Imgurl { get; set; } = null!;

    public string Title { get; set; } = null!;

    public string Description { get; set; } = null!;

    public string Projectlink { get; set; } = null!;

    public string Sourcecodelink { get; set; } = null!;
}
