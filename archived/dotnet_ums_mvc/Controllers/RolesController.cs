using DotnetUserManagementSystem.Contexts;
using DotnetUserManagementSystem.Entities;
using DotnetUserManagementSystem.Models;
using DotnetUserManagementSystem.Utilities;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc;
using Newtonsoft.Json;

namespace DotnetUserManagementSystem.Controllers;

[Authorize(Roles = Constants.ADMIN)]
public class RolesController : Controller
{
    private readonly RoleManager<ApplicationRole> roleManager;

    public RolesController(RoleManager<ApplicationRole> roleManager)
    {
        this.roleManager = roleManager;
    }

    public IActionResult AddRole()
    {
        return View();
    }

    [HttpPost]
    public async Task<IActionResult> AddRole(RoleVM vm)
    {
        if (!ModelState.IsValid)
        {
            var errors = ModelState.GetErrors<RolesController>();
            TempData[TempDataKeys.ALERT_ERROR] = string.Join("$$$", errors);
            return View();
        }

        var result = await roleManager.CreateAsync(new ApplicationRole {
            Name = vm.RoleName,
            IsActive = vm.IsActive,
        });

        if (!result.Succeeded)
        {
            TempData[TempDataKeys.ALERT_ERROR] = string.Join("$$$", result.Errors.Select(e=>e.Description));
        }

        TempData[TempDataKeys.ALERT_SUCCESS] = $"{vm.RoleName} role has been created.";
        return RedirectToAction(nameof(Index));
    }
    
    public async Task<IActionResult> DeleteRole([FromQuery] string roleId)
    {
        var role = await roleManager.FindByIdAsync(roleId);
        // System.Console.WriteLine(role!.Name);
        var result = await roleManager.DeleteAsync(role!);

        if (!result.Succeeded)
        {
            return BadRequest("failed to delete role");
        }

        return Json("success");
    }

    public async Task<IActionResult> UpdateRoleDetail([FromBody] dynamic obj)
    {
        RoleVM model = JsonConvert.DeserializeObject<RoleVM>(obj.ToString());
        var role = await roleManager.FindByIdAsync(model.Id);
        role.Name = model.RoleName;
        role.IsActive = model.IsActive;
        
        var result = await roleManager.UpdateAsync(role);

        if (!result.Succeeded)
        {
            return BadRequest("failed to update role");
        }

        return Json("success");
    }

    public ActionResult Index()
    {
        ViewBag.Roles = roleManager.Roles.AsEnumerable();

        // display all roles
        return View();
    }

}
