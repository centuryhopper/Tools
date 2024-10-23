using Microsoft.AspNetCore.Mvc;

namespace LeoPasswordManager.ViewComponents;

public class HelperPasswordFormVC : ViewComponent
{
    public async Task<IViewComponentResult> InvokeAsync(string formId, string addRowId, string addRowFieldId)
    {
        ViewBag.FormId = formId;
        ViewBag.AddRowId = addRowId;
        ViewBag.AddRowFieldId = addRowFieldId;
        return View();
    }

}