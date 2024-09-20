using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Portfolio.Entities;
using Portfolio.Repositories;

namespace Portfolio.Controllers;


public class ProjectsController : Controller
{
    private readonly IProjectsDataRepository<ProjectCardModel> ProjectRepo;

    public ProjectsController(IProjectsDataRepository<ProjectCardModel> ProjectRepo)
    {
        this.ProjectRepo = ProjectRepo;
    }

    public async Task<IActionResult> Index()
    {
        ViewBag.ProjectList = await ProjectRepo.GetDataAsync("");
        return View();
    }

    [HttpGet]
    public async Task<IActionResult> GetFilteredProjectLst([FromQuery] string searchTerm)
    {
        var data = await ProjectRepo.GetDataAsync(searchTerm);
        return Json(data);
    }

    [HttpPost]
    [Route("post-project")]
    [Authorize]
    public async Task<IActionResult> AddAsync([FromBody] ProjectCardModel model)
    {
        var data = await ProjectRepo.AddProjectAsync(model);

        if (!data.flag)
        {
            return BadRequest(data);
        }

        return Ok(data);
    }
}