using System.Diagnostics;
using System.Net.NetworkInformation;
using Microsoft.Extensions.Configuration;
using System.Runtime.InteropServices;
using dotnet_bot.Bot;
using dotnet_bot.Utils;
using Newtonsoft.Json;
using System.Reflection;
using Microsoft.Extensions.Hosting;
using AngleSharp.Dom;

string currentDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
Directory.SetCurrentDirectory(currentDirectory);
// get the .env file from where the .dll file is located
DotNetEnv.Env.Load(Path.Combine(currentDirectory, "secrets.env"));
void print(params object[] vals) => Console.WriteLine(string.Join(",", vals));

var input = "";
while (input != "1" && input != "2")
{
    System.Console.WriteLine("Pick an account to run (type 1 or 2):");
    input = Console.ReadLine();
}

int accountNumber = int.Parse(input);
string username = "";
string password = "";
string uniqueName = "";

// Retrieve environment variables based on the account number
switch (accountNumber)
{
    case 1:
        {
            string account1 = Environment.GetEnvironmentVariable("account1");
            if (account1 != null)
            {
                string[] accountDetails = account1.Split(',');
                username = accountDetails[0];
                password = accountDetails[1];
                uniqueName = accountDetails[2];
            }
            else
            {
                throw new Exception("Environment variable 'account1' not found.");
            }
            break;
        }
    case 2:
        {
            string account2 = Environment.GetEnvironmentVariable("account2");
            if (account2 != null)
            {
                string[] accountDetails = account2.Split(',');
                username = accountDetails[0];
                password = accountDetails[1];
                uniqueName = accountDetails[2];
            }
            else
            {
                throw new Exception("Environment variable 'account2' not found.");
            }
            break;
        }
    default:
        throw new Exception("Something went wrong...");
}


var emailSender = Environment.GetEnvironmentVariable("smtp_client")?.Split("|");
var receivers = Environment.GetEnvironmentVariable("smtp_receivers")?.Split("|");


async Task ProcessAccounts(string username, string password, string uniqueName)
{
    TinderBot bot =
        new(
            username,
            password,
            uniqueName,
            configsPath: "./configs"
        );

    if (bot.TryLogin())
        bot.AutoSwipe();

    // Close the browser
    bot.Dispose();

    Helpers.SendEmail(
        subject: "tdr_bot",
        body: $"finished bot for {uniqueName}",
        emailSender![0],
        emailSender[1],
        receivers: receivers!
    );

}

// single threaded way may be slower but seems to be more reliable for
// headless mode
await ProcessAccounts(username, password, uniqueName);










































// Dictionary<string, AccountConfig>? GetJson()
// {
//     string filePath = Path.Combine("./configs", "config.json");

//     // print(filePath);
//     if (File.Exists(filePath))
//     {
//         var jsonStr = File.ReadAllText(filePath);

//         Dictionary<string, AccountConfig> data = JsonConvert.DeserializeObject<
//             Dictionary<string, AccountConfig>
//         >(jsonStr)!;

//         return data;
//     }

//     return null;
// }




// format example of time stamp:
// 2023-10-20-17:50:06

// var parallelOptions = new ParallelOptions { MaxDegreeOfParallelism = 6 };

// await Parallel.ForEachAsync(
//     accounts!,
//     parallelOptions,
//     async (User account, CancellationToken _) =>
//     {
//         await ProcessAccounts(account.UserName, account.Password,
//         account.Name);
//     }
// );
