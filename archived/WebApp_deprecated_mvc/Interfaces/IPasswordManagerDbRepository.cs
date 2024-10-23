using Microsoft.AspNetCore.Http;
using LeoPasswordManager.Models;
using LeoPasswordManager.Contexts;
using LeoPasswordManager.Entities;

namespace LeoPasswordManager.Interfaces;

public interface IPasswordManagerDbRepository<T>
{
    Task<ServiceResponse> MarkUserLoggedIn(string umsUserId);
    Task<ServiceResponse> MarkUserLoggedOut(string umsUserId);
    Task<PasswordManagerUserVM?> GetPasswordManagerUser(string umsUserId);
    Task<PasswordManagerUserVM?> UpdatePasswordManagerUser(PasswordManagerUserVM vm);
    Task<PasswordManagerUserVM?> CreatePasswordManagerUser(ApplicationUser umsUser);
    Task<PasswordAccountModel?> GetAccountModelAsync(int id, int userId);
    Task<IEnumerable<T>> GetAllAccountsAsync(int userId);
    Task<T?> CreateAsync(T model);
    Task<T?> UpdateAsync(T model);
    Task<T?> DeleteAsync(T model);
    Task<UploadStatus> UploadCsvAsync(IFormFile file, int userid);
}