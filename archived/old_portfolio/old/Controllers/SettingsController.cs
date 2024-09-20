using Microsoft.AspNetCore.Mvc;

namespace Portfolio.Controllers;

public class SettingsController : Controller
{
    // GET: SettingsController
    public ActionResult Index()
    {
        return View();
    }

}
