using Microsoft.AspNetCore.Authentication.Cookies;
using LeoPasswordManager.Contexts;
using LeoPasswordManager.Interfaces;
using LeoPasswordManager.Repositories;
using LeoPasswordManager.Models;
using Microsoft.EntityFrameworkCore;
using Microsoft.AspNetCore.Identity;
using LeoPasswordManager.Entities;
using NLog;
using NLog.Web;


/*
dotnet ef dbcontext scaffold "Name=ConnectionStrings:DB_CONN" Npgsql.EntityFrameworkCore.PostgreSQL -t passwordmanager_users -t passwordmanager_accounts -o Entities -c PasswordManagerDbContext --context-dir Contexts -f

bulk insert:
\copy table_name FROM 'path/to/csv_file' WITH (FORMAT csv, HEADER true);

in the PasswordManagerDbContext, make sure this is commented out or removed otherwise the deployed version of this app won't work
protected override void OnConfiguring(DbContextOptionsBuilder optionsBuilder)
    => optionsBuilder.UseNpgsql("Name=ConnectionStrings:DB_CONN");
*/


// MUST HAVE IT LIKE THIS FOR NLOG TO RECOGNIZE DOTNET USER-SECRETS INSTEAD OF HARDCODED DELIMIT PLACEHOLDER VALUE FROM APPSETTINGS.JSON

#if DEBUG
var logger = LogManager.Setup().LoadConfigurationFromFile("nlog_dev.config").GetCurrentClassLogger();
#else
    var logger = LogManager.Setup().LoadConfigurationFromFile("nlog.config").GetCurrentClassLogger();
#endif


try
{

    var builder = WebApplication.CreateBuilder(args);

    // Add services to the container.
    builder.Logging.ClearProviders();
    builder.Host.UseNLog();
    builder.Services.AddSession(options =>
    {
        options.IdleTimeout = TimeSpan.FromHours(2);
    });

    builder
        .Services.AddAuthentication(CookieAuthenticationDefaults.AuthenticationScheme)
        .AddCookie(options =>
        {
            options.Cookie.SameSite = SameSiteMode.Strict;
            options.Cookie.SecurePolicy = CookieSecurePolicy.SameAsRequest;
            options.Cookie.HttpOnly = true;
        });

    builder.Services.Configure<CookieAuthenticationOptions>(config =>
    {
        config.AccessDeniedPath = new PathString("/AccessDenied");
        config.Events = new CookieAuthenticationEvents
        {
            OnRedirectToAccessDenied = context =>
            {
                // Custom logic when redirecting to access denied page
                context.Response.Redirect("/AccessDenied");
                return Task.CompletedTask;
            },
            // Add other event handlers as needed
        };
    });

    builder.Services.AddAuthorization(options =>
    {
        options.AddPolicy("ADMIN", policy => policy.RequireRole("Admin"));
        // options.AddPolicy("USER", policy => policy.RequireRole("User"));
    });

    // builder.Services.AddServerSideBlazor();
    builder.Services.AddControllersWithViews();
    builder.Services.AddRazorPages();
    builder.Services.AddHttpClient();
    builder.Services.AddHttpContextAccessor();

    builder.Services.AddDbContext<UserManagementContext>(options =>
        options.UseNpgsql(
            builder.Environment.IsDevelopment()
                    ?
                        builder.Configuration.GetConnectionString("UserManagementDB")
                    :
                        Environment.GetEnvironmentVariable("UserManagementDB"))
        );

    builder.Services.AddDbContext<PasswordManagerDbContext>(options =>
        options.UseNpgsql(
            builder.Environment.IsDevelopment()
                    ?
                        builder.Configuration.GetConnectionString("DB_CONN")
                    :
                        Environment.GetEnvironmentVariable("DB_CONN"))
        );

    builder.Services.AddIdentity<ApplicationUser, ApplicationRole>()
    .AddEntityFrameworkStores<UserManagementContext>()
    .AddDefaultTokenProviders();


    builder.Services.Configure<IdentityOptions>(options =>
    {
        options.Password.RequiredLength = 10;
        options.Password.RequiredUniqueChars = 3;
        options.Password.RequireNonAlphanumeric = false;
        options.SignIn.RequireConfirmedAccount = true;
    });

    builder.Services.AddSingleton<EncryptionContext>();
    builder.Services.AddScoped<
        IPasswordManagerDbRepository<PasswordAccountModel>,
        PasswordManagerDbRepository>();

    if (!builder.Environment.IsDevelopment())
    {
        var port = Environment.GetEnvironmentVariable("PORT") ?? "8081";
        builder.WebHost.UseUrls($"http://*:{port}");
    }

    var app = builder.Build();

    //app.UseForwardedHeaders(new ForwardedHeadersOptions
    //{
    //    ForwardedHeaders = ForwardedHeaders.XForwardedFor | ForwardedHeaders.XForwardedProto
    //});

    // Configure the HTTP request pipeline.
    if (!app.Environment.IsDevelopment())
    {
        app.UseExceptionHandler("/Home/Error");
        // The default HSTS value is 30 days. You may want to change this for production scenarios, see https://aka.ms/aspnetcore-hsts.
        app.UseHsts();
    }

    app.UseHttpsRedirection();
    app.UseStaticFiles();

    app.UseSession();
    app.UseRouting();

    app.UseAuthentication();
    app.UseAuthorization();

    app.MapControllerRoute(name: "default", pattern: "{controller=Account}/{action=Login}");
    // app.MapControllerRoute(name: "default", pattern: "{controller=Home}/{action=Index}/{id?}");
    app.MapRazorPages();
    // app.MapBlazorHub();

    app.Run();
}
catch (Exception ex)
{
    logger.Error(ex, "Stopped program because of exception: " + ex);
    throw ex;
}
finally {
    LogManager.Shutdown();
}