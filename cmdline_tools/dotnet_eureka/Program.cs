using System;
using System.CommandLine.Invocation;
using System.CommandLine.Binding;
using DotNetEnv;
using System.CommandLine;
using System.Diagnostics;
using System.Text;
using Newtonsoft.Json;
using Octokit;



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
              name: "--repoName",
              description: "The name of your repository");

        var githubUserName = new Option<string>(
              name: "--username",
              description: "Your github username");

        var rootCommand = new RootCommand();
        rootCommand.Add(repoPath);
        rootCommand.Add(isPrivateOption);
        rootCommand.Add(repoName);
        rootCommand.Add(githubUserName);

        rootCommand.SetHandler(async (string path, string name, bool isPrivate, string githubUserName) =>
            {
                await DoRootCommand(path, name, isPrivate, githubUserName);
            }, repoPath, repoName,isPrivateOption, githubUserName
        );

        await rootCommand.InvokeAsync(args);
    }

    public static async Task DoRootCommand(string path, string name, bool isPrivate, string githubUserName)
    {
        #region initialize parameters
        string accessToken = Environment.GetEnvironmentVariable("githubAPIToken")!;
        var client = new GitHubClient(new ProductHeaderValue("testing"))
        {
            Credentials = new Credentials(Environment.GetEnvironmentVariable("githubAPIToken")!)
        };
        var filename = $"{DateTime.Now.ToString("yyyy_mm_dd_HH_MM_ss")}.txt";
        var filePath = Path.Combine(path, name);
        var fullPathToFile = Path.Combine(filePath, filename);
        #endregion

        #region temporary comment for now
        // var editor = Environment.GetEnvironmentVariable("EDITOR");
        // if (string.IsNullOrEmpty(editor))
        // {
        //     Console.WriteLine("EDITOR environment variable not set.");
        //     return;
        // }
        #endregion


        #region create local repo and make the link to remote repo
        if (!Directory.Exists(filePath))
        {
            Directory.CreateDirectory(filePath);
        }

        Directory.SetCurrentDirectory(filePath);

        bool doesRemoteRepoExist = true;

        try
        {
            // Check if the repository already exists
            var existingRepo = await client.Repository.Get(githubUserName, name);

            Console.WriteLine($"The repository {existingRepo.Name} already exists with ID {existingRepo.Id}");
        }
        catch (NotFoundException ex)
        {
            Console.WriteLine("The repository does not exist.");
            doesRemoteRepoExist = false;

            #region remote github repo setup
            // Create a new repository
            var newRepo = new NewRepository(name)
            {
                Private = isPrivate,
                Description = "This is a new repository created using the .Net GitHub API Helper, OctoKit",
                AutoInit = false // Initialize the repository with a README file
            };
            var repo = await client.Repository.Create(newRepo);

            Console.WriteLine($"Created repository {repo.Name} with ID {repo.Id}");

            Process.Start("git", "init").WaitForExit();
            Process.Start("git", $"remote add origin git@github.com:{githubUserName}/{name}.git").WaitForExit();
            File.WriteAllText("README.md", $"# {name}");
            File.AppendAllText(".gitignore", Environment.NewLine + ".DS_Store" + Environment.NewLine + "__pycache__" + Environment.NewLine + "secrets.py");
            #endregion

        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error: {ex.Message}");
            return;
        }

        // create a new file and add content to it
        var editProcess = Process.Start(new ProcessStartInfo
        {
            FileName = "code",
            Arguments = fullPathToFile,
            UseShellExecute = true,
            WorkingDirectory = filePath,
        });

        // Create a FileSystemWatcher to monitor the directory
        var watcher = new FileSystemWatcher(filePath);

        // Set the filter to the file name
        watcher.Filter = Path.GetFileName(fullPathToFile);

        // Wait for a file change event to occur
        watcher.WaitForChanged(WatcherChangeTypes.Changed);

        // Code below this line will only execute after the file has been saved
        Console.WriteLine("File has been saved");

        // Clean up the watcher
        watcher.Dispose();


        Process.Start("git", "add .").WaitForExit();
        Process.Start("git", "commit -m \"Initial Commit\"").WaitForExit();

        if (!doesRemoteRepoExist)
        {
            Process.Start("git", "branch -m main").WaitForExit();
        }
        Process.Start("git", "push -u origin main").WaitForExit();

        #endregion

    }
}
