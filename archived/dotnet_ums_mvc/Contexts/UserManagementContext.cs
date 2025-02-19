using System;
using System.Collections.Generic;
using DotnetUserManagementSystem.Entities;
using Microsoft.AspNetCore.Identity.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore;


namespace DotnetUserManagementSystem.Contexts;

public partial class UserManagementContext : IdentityDbContext<ApplicationUser, ApplicationRole, string>
{
    public UserManagementContext()
    {
    }

    public UserManagementContext(DbContextOptions<UserManagementContext> options)
        : base(options)
    {
    }


    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        base.OnModelCreating(modelBuilder);
    }
}
