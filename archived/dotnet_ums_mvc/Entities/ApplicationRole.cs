
using Microsoft.AspNetCore.Identity;

namespace DotnetUserManagementSystem.Entities;

public class ApplicationRole : IdentityRole
{
    public bool IsActive { get; set; }
}