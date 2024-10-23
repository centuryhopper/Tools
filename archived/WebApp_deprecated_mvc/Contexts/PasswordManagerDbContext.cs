using System;
using System.Collections.Generic;
using LeoPasswordManager.Entities;
using Microsoft.EntityFrameworkCore;

namespace LeoPasswordManager.Contexts;

public partial class PasswordManagerDbContext : DbContext
{
    public PasswordManagerDbContext()
    {
    }

    public PasswordManagerDbContext(DbContextOptions<PasswordManagerDbContext> options)
        : base(options)
    {
    }

    public virtual DbSet<PasswordmanagerAccount> PasswordmanagerAccounts { get; set; }

    public virtual DbSet<PasswordmanagerUser> PasswordmanagerUsers { get; set; }

    // make sure this is commented out or removed otherwise the deployed version of this app won't work
    // protected override void OnConfiguring(DbContextOptionsBuilder optionsBuilder)
    //     => optionsBuilder.UseNpgsql("Name=ConnectionStrings:DB_CONN");

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<PasswordmanagerAccount>(entity =>
        {
            entity.HasKey(e => e.Id).HasName("passwordmanager_accounts_pkey");

            entity.ToTable("passwordmanager_accounts");

            entity.Property(e => e.Id).HasColumnName("id");
            entity.Property(e => e.CreatedAt)
                .HasColumnType("timestamp without time zone")
                .HasColumnName("created_at");
            entity.Property(e => e.LastUpdatedAt)
                .HasColumnType("timestamp without time zone")
                .HasColumnName("last_updated_at");
            entity.Property(e => e.Password)
                .HasMaxLength(256)
                .HasColumnName("password");
            entity.Property(e => e.Title)
                .HasMaxLength(256)
                .HasColumnName("title");
            entity.Property(e => e.Userid).HasColumnName("userid");
            entity.Property(e => e.Username)
                .HasMaxLength(256)
                .HasColumnName("username");

            entity.HasOne(d => d.User).WithMany(p => p.PasswordmanagerAccounts)
                .HasForeignKey(d => d.Userid)
                .OnDelete(DeleteBehavior.ClientSetNull)
                .HasConstraintName("passwordmanager_accounts_userid_fkey");
        });

        modelBuilder.Entity<PasswordmanagerUser>(entity =>
        {
            entity.HasKey(e => e.Id).HasName("passwordmanager_users_pkey");

            entity.ToTable("passwordmanager_users");

            entity.Property(e => e.Id).HasColumnName("id");
            entity.Property(e => e.Datecreated)
                .HasColumnType("timestamp without time zone")
                .HasColumnName("datecreated");
            entity.Property(e => e.Datelastlogin)
                .HasColumnType("timestamp without time zone")
                .HasColumnName("datelastlogin");
            entity.Property(e => e.Datelastlogout)
                .HasColumnType("timestamp without time zone")
                .HasColumnName("datelastlogout");
            entity.Property(e => e.Dateretired)
                .HasColumnType("timestamp without time zone")
                .HasColumnName("dateretired");
            entity.Property(e => e.Email)
                .HasMaxLength(256)
                .HasColumnName("email");
            entity.Property(e => e.Firstname)
                .HasMaxLength(256)
                .HasColumnName("firstname");
            entity.Property(e => e.Lastname)
                .HasMaxLength(256)
                .HasColumnName("lastname");
            entity.Property(e => e.UmsUserid)
                .HasMaxLength(450)
                .HasDefaultValueSql("'default_ums_id'::character varying")
                .HasColumnName("ums_userid");
        });

        OnModelCreatingPartial(modelBuilder);
    }

    partial void OnModelCreatingPartial(ModelBuilder modelBuilder);
}
