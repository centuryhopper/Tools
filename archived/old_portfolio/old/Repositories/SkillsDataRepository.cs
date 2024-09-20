using Business.DTOs;

using Microsoft.EntityFrameworkCore;
using Portfolio.Contexts;
using Portfolio.Entities;

namespace Portfolio.Repositories;

public class SkillsDataRepository : ISkillsDataRepository<SkillModel>
{
    private readonly PortfolioDBContext neondbContext;

    public SkillsDataRepository(PortfolioDBContext neondbContext)
    {
        this.neondbContext = neondbContext;
    }

    public async Task<ServiceResponse> AddSkillsAsync(SkillModel model)
    {
        var skillModel = new Skill
        {
            Title = model.Title,
        };

        try
        {
            await neondbContext.Skills.AddAsync(skillModel);
            await neondbContext.SaveChangesAsync();

            int mostRecentInsertedSkillModel = await neondbContext.Skills.MaxAsync(skill => skill.Id);

            var skillDescriptions = model.Descriptions.Select(desc => new SkillDescription
            {
                Id = mostRecentInsertedSkillModel,
                Description = desc,
            });

            await neondbContext.SkillDescriptions.AddRangeAsync(skillDescriptions);
            await neondbContext.SaveChangesAsync();

            return new ServiceResponse(flag: true, message: "Added skills and models");
        }
        catch (Exception _)
        {
            return new ServiceResponse(flag: false, message: "Error adding skills and/or skill decriptions");
        }
    }

    public async Task<IEnumerable<SkillModel>> GetDataAsync()
    {
        var skills = await neondbContext.Skills.Include(skill => skill.SkillDescriptions).Select(
            skill =>
            new SkillModel
            {
                Title = skill.Title,
                Descriptions = skill.SkillDescriptions.Select(sd => sd.Description)
            }).ToListAsync();

        return skills;
    }
}



