using Business.DTOs;

using Microsoft.EntityFrameworkCore;
using Portfolio.Contexts;
using Portfolio.Entities;

namespace Portfolio.Repositories;

public class ProjectsDataRepository : IProjectsDataRepository<ProjectCardModel>
{
    private readonly PortfolioDBContext neondbContext;

    public ProjectsDataRepository(PortfolioDBContext neondbContext)
    {
        this.neondbContext = neondbContext;
    }

    public async Task<ServiceResponse> AddProjectAsync(ProjectCardModel model)
    {
        var projectCard = new ProjectCard
        {
            Imgurl = model.ImgUrl,
            Title = model.Title,
            Description = model.Description,
            Projectlink = model.ProjectLink,
            Sourcecodelink = model.SourceCodeLink
        };

        try
        {
            await neondbContext.ProjectCards.AddAsync(projectCard);
            await neondbContext.SaveChangesAsync();
            return new ServiceResponse(flag: true, message: "Added user's message");
        }
        catch (Exception _)
        {
            return new ServiceResponse(flag: false, message: "Error adding user's message");
        }
    }

    public async Task<IEnumerable<ProjectCardModel>> GetDataAsync(string? searchTerm)
    {
        List<ProjectCard> projectCards = new();

        if (!string.IsNullOrEmpty(searchTerm))
        {
            projectCards = await neondbContext.ProjectCards.Where(p => p.Title.ToLower().Contains(searchTerm.ToLower())).AsNoTracking().ToListAsync();
        }
        else
        {
            projectCards = await neondbContext.ProjectCards.AsNoTracking().ToListAsync();
        }

        return projectCards.Select(c => new ProjectCardModel
        {
            ImgUrl = c.Imgurl,
            Title = c.Title,
            Description = c.Description,
            ProjectLink = c.Projectlink,
            SourceCodeLink = c.Sourcecodelink,
        });
    }

}



