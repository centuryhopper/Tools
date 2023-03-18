using System;
using System.CommandLine.Invocation;
using System.CommandLine.Binding;
using DotNetEnv;
using System.CommandLine;
using System.Diagnostics;
using System.Text;
using Newtonsoft.Json;


/*

TODO:

dotnet run --file [path (default is ~/.config/dotnet_eureka/]

*/


public class Program
{
    internal static async Task Main(string[] args)
    {
        DotNetEnv.Env.Load();

        var repoPath = new Option<string>(
              name: "--path",
              description: "The absolute path of your repository. An empty folder is recommended.",
              getDefaultValue: () => $"/home/{Environment.UserName}/.config/dotnet_eureka/");

        var isPrivateOption = new Option<bool>(
            name: "--private",
            description: "Decide whether the created repository should be private. Default is false.");

        var repoName = new Option<string>(
              name: "--name",
              description: "The name of your repository");

        var rootCommand = new RootCommand();
        rootCommand.Add(repoPath);
        rootCommand.Add(isPrivateOption);
        rootCommand.Add(repoName);

        rootCommand.SetHandler((string path, string name, bool isPrivate) =>
            {
                DoRootCommand(path, name, isPrivate);
            }, repoPath, repoName,isPrivateOption
        );

        await rootCommand.InvokeAsync(args);
    }

    public static void DoRootCommand(in string path, in string name, in bool isPrivate)
    {
        // System.Console.WriteLine(path);

        if (!Directory.Exists(path))
        {
            Directory.CreateDirectory(path);
        }

        #region remote github repo setup

        string accessToken = Environment.GetEnvironmentVariable("githubAPIToken")!;
        var jsonData = new { name=name, @private=isPrivate };
        HttpContent content = new StringContent(JsonConvert.SerializeObject(jsonData), Encoding.UTF8, "application/json");
        var httpClient = new HttpClient();
        httpClient.DefaultRequestHeaders.Add("Authorization", "token " + accessToken);
        httpClient.DefaultRequestHeaders.Add("Accept", "application/vnd.github.v3+json");
        var response = httpClient.PostAsync("https://api.github.com/user/repos/", content ).Result;

        #endregion

        #region temporary comment for now
        // var editor = Environment.GetEnvironmentVariable("EDITOR");
        // if (string.IsNullOrEmpty(editor))
        // {
        //     Console.WriteLine("EDITOR environment variable not set.");
        //     return;
        // }
        #endregion

        var directory = path;
        var filename = $"{DateTime.Now.ToString("yyyy_mm_dd_HH_MM_ss")}.txt";
        var filepath = Path.Combine(directory, filename);


        #region create local repo and make the link to remote repo
        try
        {
            Directory.SetCurrentDirectory(path);
            Directory.CreateDirectory(name);
            Directory.SetCurrentDirectory(name);
            Process.Start("git", "init").WaitForExit();
            Process.Start("git", $"remote add origin git@github.com:leozhang1/{name}.git").WaitForExit();
            File.WriteAllText("README.md", $"# {name}");
            File.AppendAllText(".gitignore", Environment.NewLine + ".DS_Store" + Environment.NewLine + "__pycache__" + Environment.NewLine + "secrets.py");
            Process.Start("git", "add .").WaitForExit();
            Process.Start("git", "commit -m 'Initial Commit'").WaitForExit();
            Process.Start("git", "branch -m main").WaitForExit();
            Process.Start("git", "push -u origin main").WaitForExit();
            Process.Start("git", "checkout -b main_laptop").WaitForExit();
            Process.Start("git", "push -u origin main_laptop").WaitForExit();
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex);
            return;
        }


        #endregion

        Process.Start(new ProcessStartInfo
        {
            FileName = "code",
            Arguments = filepath,
            UseShellExecute = true,

            // TODO: build the path to the local repo
            // WorkingDirectory = path
        });

    }
}

