using System;
using System.Collections.Generic;

namespace Portfolio.Contexts;

public partial class SkillDescription
{
    public int Id { get; set; }

    public int SkillId { get; set; }

    public string Description { get; set; } = null!;

    public virtual Skill Skill { get; set; } = null!;
}
