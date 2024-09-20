
using System.Security.Claims;
using Microsoft.AspNetCore.Mvc;
using Newtonsoft.Json;
using Portfolio.Entities;
using Portfolio.Repositories;
using Portfolio.Utilities;


namespace Portfolio.Controllers;


public class ContactsController : Controller
{
    private readonly IContactsDataRepository<ContactMeModel> contactRepo;
    private readonly ILogger<ContactsController> logger;
    private readonly IConfiguration configuration;
    private readonly IHostEnvironment env;

    public ContactsController(IContactsDataRepository<ContactMeModel> contactRepo, ILogger<ContactsController> Logger, IConfiguration configuration, IHostEnvironment env)
    {
        this.contactRepo = contactRepo;
        logger = Logger;
        this.configuration = configuration;
        this.env = env;
    }

    public IActionResult Index()
    {
        var previousInfo = HttpContext.Session.GetString("CONTACT") ?? "";

        ContactMeModel? vm = string.IsNullOrEmpty(previousInfo) ? null : JsonConvert.DeserializeObject<ContactMeModel>(previousInfo);

        HttpContext.Session.Remove("CONTACT");
        return View(vm);
    }

    // [HttpGet]
    // [Route("get-contacts")]
    // public async Task<IActionResult> GetAsync()
    // {
    //     var contacts = await contactRepo.GetContactsAsync();
    //     return Ok(contacts);
    // }

    [HttpPost]
    public async Task<IActionResult> Index(ContactMeModel vm)
    {
        /*
        {
        "name": "scooby doo",
        "email": "scooby_doo@mystery_inc.org",
        "subject": "scooby snacks",
        "message": "ree hee hee rooby shnacks!"
        }
        */
        if (!ModelState.IsValid)
        {
            var errors = Helpers.GetErrors<ContactsController>(ModelState);
            TempData[TempDataKeys.ALERT_ERROR] = string.Join("$$$", errors);

            HttpContext.Session.SetString("CONTACT", JsonConvert.SerializeObject(vm, Formatting.Indented));
            return RedirectToAction(nameof(Index));
        }

        // TODO: have a check to avoid users from spamming you with messages

        var data = await contactRepo.PostDataAsync(vm);

        if (!data.flag)
        {
            TempData[TempDataKeys.ALERT_ERROR] = data.message;
            HttpContext.Session.SetString("CONTACT", JsonConvert.SerializeObject(vm, Formatting.Indented));
            return RedirectToAction(nameof(Index));
        }

        var smtpInfo = env.IsDevelopment() ? configuration.GetConnectionString("smtp_client").Split("|") : Environment.GetEnvironmentVariable("smtp_client").Split("|");

        var userEmail = User.FindFirstValue(ClaimTypes.Name);

        Helpers.SendEmail(subject: "email from an potential business partner", senderEmail: smtpInfo[0], senderPassword: smtpInfo[1], body: @$"
        Name: {vm.Name}
        Email: {vm.Email}
        Subject: {vm.Subject}
        Mesage: {vm.Message}
        ", receivers: [userEmail!]);

        TempData[TempDataKeys.ALERT_SUCCESS] = "Thank you for your interest. Your message has been sent!";

        return RedirectToAction(nameof(Index));
    }
}

