using Microsoft.AspNetCore.Identity;
using Microsoft.EntityFrameworkCore;
using Portfolio.Contexts;
using Portfolio.Entities;
using Portfolio.Repositories;


/*


dotnet ef dbcontext scaffold "Name=ConnectionStrings:DB_CONN" Npgsql.EntityFrameworkCore.PostgreSQL -t passwordmanager_users -t passwordmanager_accounts -o Entities -c PasswordManagerDbContext --context-dir Contexts -f

bulk insert:
\copy table_name FROM 'path/to/csv_file' WITH (FORMAT csv, HEADER true);

in the PasswordManagerDbContext, make sure this is commented out or removed otherwise the deployed version of this app won't work
protected override void OnConfiguring(DbContextOptionsBuilder optionsBuilder)
    => optionsBuilder.UseNpgsql("Name=ConnectionStrings:DB_CONN");


#if DEBUG
var logger = LogManager.Setup().LoadConfigurationFromFile("nlog_dev.config").GetCurrentClassLogger();
#else
    var logger = LogManager.Setup().LoadConfigurationFromFile("nlog.config").GetCurrentClassLogger();
#endif

*/

TimeSpan sessionExpiration = TimeSpan.FromHours(1);


var builder = WebApplication.CreateBuilder(args);

builder.Services.AddDistributedMemoryCache();
builder.Services.AddSession(options => {
    options.IdleTimeout = sessionExpiration;
});

// Add services to the container.
builder.Services.AddControllersWithViews();

builder.Services.AddHttpContextAccessor();

// Configure the Identity database context
builder.Services.AddDbContext<ApplicationDbContext>(options =>
    options.UseNpgsql(
        builder.Environment.IsDevelopment()
                ?
                    builder.Configuration.GetConnectionString("UserManagementDB")
                :
                    Environment.GetEnvironmentVariable("UserManagementDB"))
        );

builder.Services.AddDbContext<PortfolioDBContext>(options => {
    options.UseNpgsql(builder.Environment.IsDevelopment()
                ?
                    builder.Configuration.GetConnectionString("PortfolioDB")
                :
                    Environment.GetEnvironmentVariable("PortfolioDB")
                ).EnableSensitiveDataLogging();
});

builder.Services.AddIdentity<ApplicationUser, ApplicationRole>()
.AddEntityFrameworkStores<ApplicationDbContext>()
.AddDefaultTokenProviders();


builder.Services.Configure<IdentityOptions>(options =>
{
    options.Password.RequiredLength = 10;
    options.Password.RequiredUniqueChars = 3;
    options.Password.RequireNonAlphanumeric = false;
    options.SignIn.RequireConfirmedAccount = true;
});


builder.Services.AddScoped<IContactsDataRepository<ContactMeModel>, ContactsDataRepository>();
builder.Services.AddScoped<IProjectsDataRepository<ProjectCardModel>, ProjectsDataRepository>();
builder.Services.AddScoped<IBlogsDataRepository<BlogModel>, BlogsDataRepository>();
builder.Services.AddScoped<ISkillsDataRepository<SkillModel>, SkillsDataRepository>();

if (!builder.Environment.IsDevelopment())
{
    var port = Environment.GetEnvironmentVariable("PORT") ?? "8081";
    builder.WebHost.UseUrls($"http://*:{port}");
}

var app = builder.Build();

// Configure the HTTP request pipeline.
if (!app.Environment.IsDevelopment())
{
    app.UseExceptionHandler("/Home/Error");
    // The default HSTS value is 30 days. You may want to change this for production scenarios, see https://aka.ms/aspnetcore-hsts.
    app.UseHsts();
}

app.UseHttpsRedirection();
app.UseStaticFiles();

app.UseDeveloperExceptionPage();

app.UseSession();

app.UseRouting();
app.UseAuthentication();

app.UseAuthorization();

app.MapControllerRoute(
    name: "default",
    pattern: "{controller=Home}/{action=Index}/{id?}");

app.Run();
