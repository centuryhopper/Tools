using System.Globalization;
using CsvHelper;
using CsvHelper.Configuration;
using Microsoft.EntityFrameworkCore;
using LeoPasswordManager.Contexts;
using LeoPasswordManager.Interfaces;
using LeoPasswordManager.Models;
using LeoPasswordManager.Utilities;
using LeoPasswordManager.Entities;


namespace LeoPasswordManager.Repositories;

public class PasswordManagerDbRepository : IPasswordManagerDbRepository<PasswordAccountModel>
{
    private readonly EncryptionContext encryptionContext;
    private readonly ILogger<PasswordManagerDbRepository> logger;
    private readonly PasswordManagerDbContext passwordManagerDbContext;

    public PasswordManagerDbRepository(EncryptionContext encryptionContext, ILogger<PasswordManagerDbRepository> logger, PasswordManagerDbContext passwordManagerDbContext)
    {
        this.encryptionContext = encryptionContext;
        this.logger = logger;
        this.passwordManagerDbContext = passwordManagerDbContext;
    }

    public async Task<PasswordAccountModel?> GetAccountModelAsync(int id, int userId)
    {
        var accountModel = await passwordManagerDbContext.PasswordmanagerAccounts.FindAsync(id);

        return accountModel?.ToPasswordManagerAccountModel();
    }

    public async Task<PasswordAccountModel?> CreateAsync(PasswordAccountModel model)
    {
        model.Password = Convert.ToBase64String(encryptionContext.Encrypt(model.Password!));
        model.CreatedAt = DateTime.Now;
        model.LastUpdatedAt = DateTime.Now;
        try
        {
            await passwordManagerDbContext.PasswordmanagerAccounts.AddAsync(model.ToPasswordManagerAccount());
            await passwordManagerDbContext.SaveChangesAsync();
        }
        catch (System.Exception ex)
        {
            return null;
        }
        return model;
    }

    public async Task<PasswordAccountModel?> DeleteAsync(PasswordAccountModel model)
    {
        var queryModel = await passwordManagerDbContext.PasswordmanagerAccounts.FindAsync(model.Id);
        passwordManagerDbContext.PasswordmanagerAccounts.Remove(queryModel!);
        await passwordManagerDbContext.SaveChangesAsync();
        return model;
    }

    public async Task<IEnumerable<PasswordAccountModel>> GetAllAccountsAsync(int userId)
    {
        var results = await passwordManagerDbContext.PasswordmanagerAccounts.AsNoTracking().Where(a => a.Userid == userId).ToListAsync();
        // var results = await PasswordAccountContext.PasswordmanagerAccounts.AsNoTracking().ToListAsync();

        if (!results.Any())
        {
            return Enumerable.Empty<PasswordAccountModel>();
        }

        return results.Select(m =>
        {
            return new PasswordAccountModel
            {
                Id = m.Id,
                Title = m.Title,
                Username = m.Username,
                Password = encryptionContext.Decrypt(Convert.FromBase64String(m.Password)).Replace(",", "$"),
                UserId = m.Userid,
                CreatedAt = m.CreatedAt,
                LastUpdatedAt = m.LastUpdatedAt
            };
        });
    }

    public int AccountsCount(int UserId, string title)
    {
        var cnt = passwordManagerDbContext.PasswordmanagerAccounts.Where(a => a.Userid == UserId && a.Title.ToLower().Contains(title)).Count();
        return cnt;
    }

    public async Task<PasswordAccountModel?> UpdateAsync(PasswordAccountModel model)
    {
        var dbModel = await passwordManagerDbContext.PasswordmanagerAccounts.FindAsync(model.Id);
        try
        {
            dbModel!.LastUpdatedAt = DateTime.Now;
            dbModel.Title = model.Title;
            dbModel.Username = model.Username;
            dbModel.Password = Convert.ToBase64String(encryptionContext.Encrypt(model.Password));
            await passwordManagerDbContext.SaveChangesAsync(); 
        }
        catch (System.Exception ex)
        {
            return null;
        }
        return model;
    }

    public async Task<UploadStatus> UploadCsvAsync(IFormFile file, int userid)
    {
        // set up csv helper and read file
        var config = new CsvConfiguration(CultureInfo.InvariantCulture)
        {
            HasHeaderRecord = true,

            // set to null to allow files with only title, usernam, and password headers to be uploaded
            HeaderValidated = null,
            MissingFieldFound = null,
        };


        using var streamReader = new StreamReader(file.OpenReadStream());
        using var csvReader = new CsvReader(streamReader, config);
        IAsyncEnumerable<PasswordAccountModel> records;



        try
        {
            csvReader.Context.RegisterClassMap<PasswordsMapper>();
            records = csvReader.GetRecordsAsync<PasswordAccountModel>();

            await foreach (var record in records)
            {
                await CreateAsync(new PasswordAccountModel
                {
                    UserId = userid,
                    Title = record.Title,
                    Username = record.Username,
                    Password = record.Password,
                    CreatedAt = DateTime.Now,
                    LastUpdatedAt = DateTime.Now,
                });
            }
        }
        catch (CsvHelperException ex)
        {
            return new UploadStatus
            {
                Message = ex.Message,
                UploadEnum = UploadEnum.FAIL
            };
        }

        return new UploadStatus
        {
            Message = "Upload csv success!",
            UploadEnum = UploadEnum.SUCCESS
        };
    }

    public async Task<PasswordManagerUserVM?> UpdatePasswordManagerUser(PasswordManagerUserVM vm)
    {
        try
        {
            var passwordManagerUser = await passwordManagerDbContext.PasswordmanagerUsers.FindAsync(vm.Id);
            passwordManagerUser!.Firstname = vm.Firstname;
            passwordManagerUser.Lastname = vm.Lastname;
            await passwordManagerDbContext.SaveChangesAsync();

            return passwordManagerUser?.ToPasswordManagerUserVM();
        }
        catch (System.Exception ex)
        {
            return null;
        }
    }

    public async Task<PasswordManagerUserVM?> GetPasswordManagerUser(string umsUserId)
    {
        var passwordManagerUser = await passwordManagerDbContext.PasswordmanagerUsers.FirstOrDefaultAsync(u=>u.UmsUserid == umsUserId);

        return passwordManagerUser?.ToPasswordManagerUserVM();
    }

    public async Task<PasswordManagerUserVM?> CreatePasswordManagerUser(ApplicationUser umsUser)
    {
        PasswordmanagerUser passwordManagerUser = new(){
            Email = umsUser.Email,
            Firstname = "test_firstname",
            Lastname = "test_firstname",
            Datelastlogin = DateTime.Now,
            Datelastlogout = null,
            Datecreated = DateTime.Now,
            Dateretired = null,
            UmsUserid = umsUser.Id,
        };

        await passwordManagerDbContext.PasswordmanagerUsers.AddAsync(passwordManagerUser);
        await passwordManagerDbContext.SaveChangesAsync();

        return passwordManagerUser.ToPasswordManagerUserVM();
    }

    public async Task<ServiceResponse> MarkUserLoggedOut(string umsUserId)
    {
        try
        {
            var user = await passwordManagerDbContext.PasswordmanagerUsers.FirstOrDefaultAsync(u=>u.UmsUserid == umsUserId);
            user.Datelastlogout = DateTime.Now;
            await passwordManagerDbContext.SaveChangesAsync();
            return new ServiceResponse(true, "log out date marked!");
        }
        catch (System.Exception ex)
        {
            return new ServiceResponse(false, "log out date unmarked!");
        }
    }

    public async Task<ServiceResponse> MarkUserLoggedIn(string umsUserId)
    {
        try
        {
            var user = await passwordManagerDbContext.PasswordmanagerUsers.FirstOrDefaultAsync(u=>u.UmsUserid == umsUserId);
            user.Datelastlogin = DateTime.Now;
            await passwordManagerDbContext.SaveChangesAsync();
            return new ServiceResponse(true, "log in date marked!");
        }
        catch (System.Exception ex)
        {
            return new ServiceResponse(false, "log in date unmarked!");
        }
    }

}

