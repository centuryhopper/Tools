using System;
using System.Diagnostics;
using System.Reflection;
using dotnet_bmbl_bot.Bot;
using dotnet_bmbl_bot.Models;
using dotnet_bmbl_bot.Utils;
using Microsoft.Extensions.Configuration;
using Newtonsoft.Json;
using OpenQA.Selenium;
using OpenQA.Selenium.Firefox;
using WebDriverManager;
using WebDriverManager.DriverConfigs.Impl;

// dotnet publish -c Release -o test

string currentDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
// string parentDirectory = Directory.GetParent(currentDirectory).FullName;
Directory.SetCurrentDirectory(currentDirectory);

// get the .env file from where the .dll file is located
DotNetEnv.Env.Load(Path.Combine(currentDirectory, "secrets.env"));

void print(params object[] vals) => Console.WriteLine(string.Join(",", vals));

var isRunningInDocker = Environment.GetEnvironmentVariable("DOTNET_RUNNING_IN_CONTAINER");


var cmdArgs = Environment.GetCommandLineArgs();

//var builder = new ConfigurationBuilder().AddUserSecrets<Program>();
//
//var configuration = builder.Build();

var getAccountsString = Environment
    .GetEnvironmentVariable("accounts");

//if (string.IsNullOrEmpty(getAccountsString))
//{
//    string currentDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
//    string parentDirectory = Directory.GetParent(currentDirectory).FullName;
//    Directory.SetCurrentDirectory(parentDirectory);
//}

var emailSender = Environment.GetEnvironmentVariable("smtp_client")?.Split("|");
//emailSender ??= configuration["smtp_client"]?.Split("|");
var receivers = Environment.GetEnvironmentVariable("smtp_receivers")?.Split("|");
//receivers = configuration["smtp_receivers"]?.Split("|");

#if DEBUG
    // Console.WriteLine("Running in Debug mode (likely locally).");
    string LOCAL_PIC_BUILD_DIR = Directory.GetCurrentDirectory();
#else
// MUST run 'dotnet build -c Release -o test' or this wont work
    // Console.WriteLine("Running in Release mode (likely in a build pipeline).");
    string LOCAL_PIC_BUILD_DIR = "/home/leo_zhang/Documents/GitHub/Bots/dotnet_bmbl_bot/test";
#endif

// System.Console.WriteLine("current DIR: " + Directory.GetCurrentDirectory());

// Console.WriteLine(LOCAL_PIC_BUILD_DIR);

var configsPath = Convert.ToBoolean(isRunningInDocker)
    ? "/app/configs"
    : Path.Combine(LOCAL_PIC_BUILD_DIR, "configs");

Directory.CreateDirectory(configsPath);

var timeStampDir = Convert.ToBoolean(isRunningInDocker)
    ? "/app/script_execution_records"
    : Path.Combine(LOCAL_PIC_BUILD_DIR, "script_execution_records");
Directory.CreateDirectory(timeStampDir);

var geckoDriverPath = Convert.ToBoolean(isRunningInDocker) ? "/usr/local/bin/geckodriver" : "./geckodriver_for_arm";




// just in case we're running from local pc cron instead of the raspberry pi
//getAccountsString ??= configuration["accounts"];


var accounts = getAccountsString
    ?.Split("|")
    .Select(acc =>
    {
        var pieces = acc.Split(",");
        string username = pieces[0],
            password = pieces[1],
            name = pieces[2];
        return new User
        {
            UserName = username,
            Password = password,
            Name = name,
        };
    })
    .ToList();

//accounts.ForEach(x => print(x));

async Task<bool> IsConnectedToWifi()
{
    ProcessStartInfo startInfo = new ProcessStartInfo
    {
        FileName = "curl",
        Arguments = $"-Is \"https://www.google.com\"",
        RedirectStandardOutput = true,
        RedirectStandardError = true,
        UseShellExecute = false,
        CreateNoWindow = true
    };

    using Process process = new Process();
    process.StartInfo = startInfo;
    process.Start();

    string standardOutput = await process.StandardOutput.ReadToEndAsync();
    string standardError = await process.StandardError.ReadToEndAsync();

    await process.WaitForExitAsync();

    return standardOutput.Contains("HTTP/2 200");
}

async Task ProcessAccounts(string username, string password, string name = "")
{
    //print("yooo");
    if (cmdArgs.Length > 2)
    {
        Console.WriteLine("Please only pass in one argument");
        return;
    }
    string lastReadDateTime = ReadLastDateTimeFromTxt(name);
    string fmt = "yyyy-MM-dd-HH:mm:ss";

    if (string.IsNullOrEmpty(lastReadDateTime))
    {
        // Epoch start time so that we're guaranteed to run the program when the
        // file is created Since it has definitely been at 24 hours since 1970
        lastReadDateTime = "1970-01-01-00:00:00";
        // File.WriteAllText($"{Environment.CurrentDirectory}/script_execution_records/{name}.txt",
        // lastReadDateTime);
    }

    DateTime lastDateTime = DateTime.ParseExact(lastReadDateTime.Trim(), fmt, null);
    TimeSpan diff = DateTime.Now - lastDateTime;
    double diffInHours = diff.TotalSeconds / 3600;
    //bool isConnectedToWifi = await IsConnectedToWifi();

    if (diffInHours < 24.1)
    {
        return;
    }

    var timeStampPath = $"{Environment.CurrentDirectory}/script_execution_records/";

    Directory.CreateDirectory(timeStampPath);

    var filename = $"{name}.txt";
    var fullPath = Path.Combine(timeStampPath, filename);

    var accounts = GetJson<Dictionary<string, AccountConfig>>(
        Path.Combine(configsPath, "config.json")
    );
    var left_swipes = GetJson<Dictionary<string, Dictionary<string, string>>>(
        Path.Combine(configsPath, "left_swipes.json")
    );

    BumbleBot bot =
        new(
            username,
            password,
            name,
            geckoDriverPath,
            configsPath,
            "https://bumble.com/get-started",
            accounts,
            left_swipes,
            isCreatingAccount: cmdArgs.Length > 1 && cmdArgs[1] != "loop"
        );

    if (cmdArgs.Length == 2)
    {
        if (cmdArgs[1] == "loop")
        {
            while (true)
            {
                bot.AutoSwipe();
                bot.Dispose();

                bot = new(
                    username,
                    password,
                    name,
                    geckoDriverPath,
                    configsPath,
                    "https://bumble.com/get-started",
                    accounts,
                    left_swipes,
                    isCreatingAccount: false
                );
            }
        }
        else
        {
            bot.InitialUploadPhotos();
            bot.InitialSetup();
            bot.FilterAgeAndDistance();
            bot.AddFiltersToAccount();
        }
    }
    else
    {
        var markDate = DateTime.Now.ToString(fmt);
        await File.WriteAllTextAsync(fullPath, markDate);
        bot.AutoSwipe();
        // bot.SendMessageToMatches();
        // mark final finish date time
        markDate = DateTime.Now.ToString(fmt);
        await File.WriteAllTextAsync(fullPath, markDate);
        Helpers.SendEmail(
            subject: "bmbl_bot",
            body: $"finished bot for {name}",
            emailSender![0],
            emailSender[1],
            receivers: receivers!
        );
    }

    bot.Dispose();
}

string? ReadLastDateTimeFromTxt(string name)
{
    string filePath = $"{Environment.CurrentDirectory}/script_execution_records/{name}.txt";
    if (File.Exists(filePath))
    {
        return File.ReadAllText(filePath);
    }

    return null;
}

T? GetJson<T>(string completeFilePath)
    where T : class
{
    T? data = null;
    try
    {
        string filePath = completeFilePath;

        if (File.Exists(filePath))
        {
            var jsonStr = File.ReadAllText(filePath);

            data = JsonConvert.DeserializeObject<T>(jsonStr)!;

            return data;
        }
    }
    catch (Exception e)
    {
        System.Console.WriteLine(e.Message);
    }

    return data;
}

// single threaded way may be slower but seems to be more reliable for
// headless mode
foreach (var account in accounts!)
{
   // if (account.Name.ToLower().Trim() == "edge")
   // {
   //     //print(account.Name);
   await ProcessAccounts(account.UserName, account.Password, account.Name.ToLower().Trim());
   // }
}

// timestamp format
// 2023-10-20-17:50:06
// dotnet run loop to run all of them.
// dotnet run [name] to create account

// System.Console.WriteLine("hello there");
