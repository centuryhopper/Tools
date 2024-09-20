using Microsoft.AspNetCore.Mvc;
using Portfolio.Entities;
using Portfolio.Repositories;



namespace Portfolio.Controllers;

public class SkillsController : Controller
{
    private readonly ISkillsDataRepository<SkillModel> SkillRepo;

    public SkillsController(ISkillsDataRepository<SkillModel> SkillRepo)
    {
        this.SkillRepo = SkillRepo;
    }

    public async Task<IActionResult> Index()
    {
        ViewBag.SkillsLst = await SkillRepo.GetDataAsync();

        return View();
    }

    [HttpPost]
    [Route("post-skill")]
    public async Task<IActionResult> AddAsync([FromBody] SkillModel model)
    {
        var data = await SkillRepo.AddSkillsAsync(model);

        if (!data.flag)
        {
            return BadRequest(data);
        }

        return Ok(data);
    }
}