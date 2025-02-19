using DotnetUserManagementSystem.Entities;
using DotnetUserManagementSystem.Utilities;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc;

namespace DotnetUserManagementSystem.Controllers;

[Authorize(Roles = Constants.ADMIN)]
public class UsersController : Controller
{
    private readonly UserManager<ApplicationUser> userManager;

    public UsersController(UserManager<ApplicationUser> userManager)
    {
        this.userManager = userManager;
    }
    
    public async Task<ActionResult> Index()
    {
        var users = userManager.Users.AsEnumerable().ToList();
        Dictionary<string,IEnumerable<string>> userRoles = new();
        foreach (var user in users)
        {
            userRoles.Add(user.Id, await userManager.GetRolesAsync(user));
        }
        ViewBag.UserRoles = userRoles;
        ViewBag.Users = users;

        return View();
    }

    public async Task<ActionResult> AddUser()
    {
        return View();
    }

    public async Task<ActionResult> RemoveUser()
    {
        return View();
    }

}
