using System;
using System.Collections.Generic;

namespace Portfolio.Contexts;

public partial class Skill
{
    public int Id { get; set; }

    public string Title { get; set; } = null!;

    public virtual ICollection<SkillDescription> SkillDescriptions { get; set; } = new List<SkillDescription>();
}
