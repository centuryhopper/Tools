using System;
using System.Collections.Generic;
using LeoPasswordManager.Entities;
using Microsoft.AspNetCore.Identity.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.Configuration;

namespace LeoPasswordManager.Contexts;

public class UserManagementContext : IdentityDbContext<ApplicationUser, ApplicationRole, string>
{
    public UserManagementContext(DbContextOptions<UserManagementContext> options)
        : base(options)
    {
    }

    protected override void OnModelCreating(ModelBuilder builder)
    {
        base.OnModelCreating(builder);
    }

    // Add other DbSets if needed
}
