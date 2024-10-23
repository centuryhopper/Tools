using System.Security.Claims;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using LeoPasswordManager.Interfaces;
using LeoPasswordManager.Models;
using LeoPasswordManager.Utilities;
using Newtonsoft.Json;
using Microsoft.AspNetCore.Identity;
using LeoPasswordManager.Entities;

namespace LeoPasswordManager.Controllers;

public class HomeController : Controller
{
    private readonly ILogger<HomeController> logger;
    private readonly IPasswordManagerDbRepository<PasswordAccountModel> passwordManagerAccountRepository;
    private readonly UserManager<ApplicationUser> userManager;

    public HomeController(ILogger<HomeController> logger, IPasswordManagerDbRepository<PasswordAccountModel> passwordManagerAccountRepository, UserManager<ApplicationUser> userManager)
    {
        this.logger = logger;
        this.passwordManagerAccountRepository = passwordManagerAccountRepository;
        this.userManager = userManager;
    }

    public IActionResult Welcome()
    {
        return View();
    }

    [Authorize, HttpPost]
    public async Task<IActionResult> UploadCSV(IFormFile file, int userId)
    {
         if (file == null || file.Length == 0)
        {
            return BadRequest(new { message = "File is missing or empty." });
        }

        // Add debug logs or console output
        // Console.WriteLine($"File Name: {file.FileName}");
        // Console.WriteLine($"File Length: {file.Length}");
        // Console.WriteLine($"User ID: {userId}");

        var result = await passwordManagerAccountRepository.UploadCsvAsync(file, userId);

        // logger.LogWarning(result.Message);
        // System.Console.WriteLine("nooo");

        if (result.UploadEnum == UploadEnum.FAIL)
        {
            return BadRequest(new {message = result.Message});
        }

        return Json(new {message = result.Message});
    }

    // [Authorize, HttpPost]
    // public async Task<IActionResult> UploadCSV(IFormFile file, int userId)
    // {
    //     var result = await passwordManagerAccountRepository.UploadCsvAsync(file, userId);

    //     if (result.UploadEnum == UploadEnum.FAIL)
    //     {
    //         return BadRequest("failed to upload csv");
    //     }

    //     return Ok("upload csv success!");
    //     // return RedirectToAction(nameof(Passwords), new {pg=1});

    // }



    [HttpPost]
    public async Task<IActionResult> UpdatePasswordDetail(PasswordAccountModel passwordAccountModel)
    {
        List<string> errors = new();
        if (!ModelState.IsValid)
        {
            errors = ModelState.GetErrors<HomeController>().ToList();
            TempData[TempDataKeys.ALERT_ERROR] = string.Join("$$$", errors);
            return RedirectToAction(nameof(Passwords));
        }

        var updatedPasswordDetail = await passwordManagerAccountRepository.UpdateAsync(passwordAccountModel);

        return RedirectToAction(nameof(Passwords));
    }

    [HttpPost]
    public async Task<IActionResult> DeletePasswordDetail(int passwordDetailId)
    {
        var umsUserId = HttpContext.User.FindFirstValue(ClaimTypes.NameIdentifier)!;
        // get user id from finding user via ums userid
        var user = await passwordManagerAccountRepository.GetPasswordManagerUser(umsUserId: umsUserId);
        var accountModel = await passwordManagerAccountRepository.GetAccountModelAsync(passwordDetailId, user!.Id);
        var deleteDetail = await passwordManagerAccountRepository.DeleteAsync(accountModel!);

        return Json(new {message = "Deletion Successful!"});
    }

    [HttpPost]
    public async Task<IActionResult> AddPasswordRows([FromBody] dynamic obj)
    {
        List<string> errors = new();
        if (!ModelState.IsValid)
        {
            errors = ModelState.GetErrors<HomeController>().ToList();
            TempData[TempDataKeys.ALERT_ERROR] = string.Join("$$$", errors);
            return RedirectToAction(nameof(Passwords));
        }

        // logger.LogWarning($"{obj.ToString()}");

        AddPasswordRowsVM model = JsonConvert.DeserializeObject<AddPasswordRowsVM>(obj.ToString())!;

        foreach (var acc in model.PasswordAccountModels)
        {
            // logger.LogWarning($"{acc.Title}");
            // logger.LogWarning($"{acc.Username}");
            // logger.LogWarning($"{acc.Password}");

            await passwordManagerAccountRepository.CreateAsync(acc);
        }

        return RedirectToAction(nameof(Passwords));
    }

    [Authorize]
    public async Task<IActionResult> Passwords(int pg = 1)
    {
        // logger.LogWarning("logged in!");
        // var umsUserId = HttpContext.User.FindFirstValue(ClaimTypes.NameIdentifier)!;
        var umsUser = await userManager.GetUserAsync(User);
        // logger.LogWarning(umsUser is null ? "couldnt find user in the ums": "ums user id: " + umsUser.Id);

        // get user id from finding user via ums userid
        var user = (await passwordManagerAccountRepository.GetPasswordManagerUser(umsUserId: umsUser!.Id)) ?? await passwordManagerAccountRepository.CreatePasswordManagerUser(umsUser);

        ViewBag.userId = user!.Id;
        // logger.LogWarning(user is null ? "password user not found" : "password user id: " + user.Id.ToString());

        const int PAGE_SIZE = 5;
        if (pg < 1) pg = 1;
        // logger.LogWarning("getting all passwords associated with " + user?.Id);
        var passwordAccounts = await passwordManagerAccountRepository.GetAllAccountsAsync(user!.Id);
        var pager = new Pager(totalItems: passwordAccounts.Count(), pageNumber: pg, pageSize: PAGE_SIZE);
        int recordsToSkip = (pg - 1) * PAGE_SIZE;
        var pagedDetails = passwordAccounts.Skip(recordsToSkip).Take(pager.PageSize);
        // pagedDetails.ToList().ForEach(x => logger.LogWarning(x.Title));

        ViewBag.PagedDetails = pagedDetails;
        ViewBag.Pager = pager;
        ViewBag.TotalRecordsCount = passwordAccounts.Count();
        ViewBag.PasswordAccounts = passwordAccounts;
        // logger.LogWarning(passwordAccounts.Count().ToString());

        return View();
    }


    [Authorize]
    public IActionResult Settings()
    {
        return View();
    }


    // public IActionResult Privacy()
    // {
    //     return View();
    // }

    // [ResponseCache(Duration = 0, Location = ResponseCacheLocation.None, NoStore = true)]
    // public IActionResult Error()
    // {
    //     return View(new ErrorViewModel { RequestId = Activity.Current?.Id ?? HttpContext.TraceIdentifier });
    // }
}
