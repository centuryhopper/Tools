using System.Diagnostics;
using System.Reflection;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Session;
using Portfolio.Models;
namespace Portfolio.Controllers;

public class HomeController : Controller
{
    private readonly ILogger<HomeController> _logger;
    public HomeController(ILogger<HomeController> logger)
    {
        _logger = logger;
    }

    private object GetInstanceField(Type type, ISession session, string v)
    {
        BindingFlags bindFlags = BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static;
        FieldInfo field = type.GetField(v, bindFlags);
        return field.GetValue(session);
    }

    // get the expiration time of session (useful for automatically logging out the user for idling for tool long)
    public int Time()
    {
        var value = GetInstanceField(typeof(DistributedSession), HttpContext.Session, "_idleTimeout").ToString();
        string[] time = value!.Split(':');
        var (hours, minutes, seconds) = (time[0], time[1], time[2]);
        var totalSeconds = int.Parse(time[0]) * 3600 + int.Parse(time[1]) * 60 + int.Parse(time[2]);
        // logger.LogWarning($"{totalSeconds}");
        return totalSeconds;
    }

    public IActionResult Index()
    {
        return View();
    }

    public IActionResult AboutMe()
    {
        return View();
    }

    public IActionResult Privacy()
    {
        return View();
    }
    
    [ResponseCache(Duration = 0, Location = ResponseCacheLocation.None, NoStore = true)]
    public IActionResult Error()
    {
        return View(new ErrorViewModel { RequestId = Activity.Current?.Id ?? HttpContext.TraceIdentifier });
    }
}
