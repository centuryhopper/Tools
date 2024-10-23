using LeoPasswordManager.Models;
using Microsoft.AspNetCore.Mvc;

namespace LeoPasswordManager.ViewComponents;

public class PasswordTablePaginationVC : ViewComponent
{
    public async Task<IViewComponentResult> InvokeAsync(PaginationParameters paginationParameters)
    {
        return View(paginationParameters);
    }

}