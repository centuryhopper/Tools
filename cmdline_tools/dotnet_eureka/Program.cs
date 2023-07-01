using System.Text;
using System.CommandLine;
using System.Diagnostics;
using Octokit;

/*
CLI tool to input and store your ideas without leaving the terminal (inspired by the eureka package from rust)

optional nuget packages:
-DotNetEnv

required nuget packages:
-Octokit
-System.CommandLine
*/

public class Program
{
    /*

    dotnet run --reponame testing_dir --username leozhang1 --filename my_secret_ideas

    */
    internal static async Task Main(string[] args)
    {
        // look for a .env file in this project
        DotNetEnv.Env.Load();

#region create arguments

        var repoPath = new Option<string>(
              name: "--path",
              description: "The absolute path of your repository. An empty folder is recommended.",
              getDefaultValue: () => $"/home/{Environment.UserName}/.config/dotnet_eureka/");

        var isPrivateOption = new Option<bool>(
            name: "--private",
            description: "Decide whether the created repository should be private. Default is false.");

        var repoName = new Option<string>(
              name: "--reponame",
              description: "The name of your repository");

        var githubUserName = new Option<string>(
              name: "--username",
              description: "Your github username");

// $EDITOR
        var editorToUse = new Option<string>(
            name: "--editor",
            description: "The editor you would like to use. Defaults to your editor set by your $EDITOR environment variable, if it exists, otherwise defaults to code",
            getDefaultValue: () =>
            {
                return Environment.GetEnvironmentVariable("EDITOR") ?? "code";
            }
        );

        var fileName = new Option<string>(
            name: "--filename",
            description: "name of the file you input your ideas in"
        );

        var view = new Option<bool>(
            name: "--view",
            description: "print out all of your stored ideas"
        );

#endregion

#region add arguments to the root command
        var rootCommand = new RootCommand();
        rootCommand.Add(repoPath);
        rootCommand.Add(isPrivateOption);
        rootCommand.Add(repoName);
        rootCommand.Add(githubUserName);
        rootCommand.Add(editorToUse);
        rootCommand.Add(fileName);
        rootCommand.Add(view);
#endregion

        // order of arguments do matter
        rootCommand.SetHandler(
            async (string repoPath, string ideaRepoName, bool isPrivate, string githubUserName, string editorToUse, string fileName, bool view) =>
            {
                // if user passes in --view
                if (view)
                {
                    await ShowIdeas(ideaRepoName, githubUserName);
                }
                else
                {
                    await RunEureka(repoPath, ideaRepoName, isPrivate, githubUserName, editorToUse, fileName);
                }
            }, repoPath, repoName, isPrivateOption, githubUserName, editorToUse, fileName, view
        );

        await rootCommand.InvokeAsync(args);
    }

    public static async Task ShowIdeas(string ideaRepoName, string githubUserName)
    {
        string accessToken = Environment.GetEnvironmentVariable("githubAPIToken")!;
        var client = new GitHubClient(new ProductHeaderValue("testing"))
        {
            Credentials = new Credentials(Environment.GetEnvironmentVariable("githubAPIToken")!)
        };

        if (!await DoesRepoExist(client, githubUserName, ideaRepoName))
        {
            return;
        }

        var fileContents = await client.Repository.Content.GetAllContents(githubUserName, ideaRepoName);

        // Print the contents of the file
        foreach (var fileContent in fileContents)
        {
            if (fileContent.Type == ContentType.File)
            {
                // Get the contents of the file
                var stuff = await client.Repository.Content.GetRawContent(githubUserName, ideaRepoName, fileContent.Path);

                // Print the contents of the file
                Console.WriteLine($"Contents of {fileContent.Path}:");

                var decodedContents = Encoding.UTF8.GetString(stuff);
                Console.WriteLine(decodedContents);
            }
        }
     }

    public static async Task RunEureka(string repoPath, string ideaRepoName, bool isPrivate, string githubUserName, string editorToUse, string fileName)
    {
        #region initialize parameters and interact with github

        string accessToken = Environment.GetEnvironmentVariable("githubAPIToken")!;

        var client = new GitHubClient(new ProductHeaderValue("testing"))
        {
            Credentials = new Credentials(accessToken)
        };

        var filePath = Path.Combine(repoPath, ideaRepoName);
        var filename = $"{fileName}_{DateTime.Now.ToString("yyyy_MM_dd_HH_mm_ss")}.txt";

        // default for my case is /home/leo_zhang/.config/dotnet_eureka/testing_dir/[file_name].txt
        var fullPathToFile = Path.Combine(filePath, filename);

        #endregion

        if (!Directory.Exists(filePath))
        {
            Directory.CreateDirectory(filePath);
        }

        // equivalent to a "cd" command in bash
        Directory.SetCurrentDirectory(filePath);

        if (!await DoesRepoExist(client, githubUserName, ideaRepoName))
        {
            // remote github repo setup
            // Create a new repository
            var newRepo = new NewRepository(ideaRepoName)
            {
                Private = isPrivate,
                Description = "This is a new repository created using the .Net GitHub API Helper, OctoKit",
                AutoInit = false
            };

            var repo = await client.Repository.Create(newRepo);

            Console.WriteLine($"Created repository {repo.Name} with ID {repo.Id}");

            // link local with remote repo
            await Process.Start("git", "init").WaitForExitAsync();
            await Process.Start("git", $"remote add origin git@github.com:{githubUserName}/{ideaRepoName}.git").WaitForExitAsync();
            await Process.Start("git", "branch -m main").WaitForExitAsync();
            await File.WriteAllTextAsync("README.md", $"# {ideaRepoName}");
            await File.AppendAllTextAsync("README.md", $"generated repo id: {repo.Id}");
        }

        // create a new file and let the user add content to it
        // so that it can be automatically pushed to the remote repository
        Process.Start(new ProcessStartInfo
        {
            FileName = editorToUse,
            Arguments = fullPathToFile,
            UseShellExecute = true,
            WorkingDirectory = filePath,
        });

        // watch for when the user saves the file
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

        //push changes to the remote
        await Process.Start("git", "add .").WaitForExitAsync();
        await Process.Start("git", "commit -m \"Initial Commit\"").WaitForExitAsync();
        await Process.Start("git", "push -u origin main").WaitForExitAsync();
    }

    private static async Task<bool> DoesRepoExist(GitHubClient client, string githubUserName, string ideaRepoName)
    {
        try
        {
            // Check if the remote repository already exists
            var existingRepo = await client.Repository.Get(githubUserName, ideaRepoName);

            Console.WriteLine($"The repository {existingRepo.Name} already exists with ID {existingRepo.Id}");

            return true;
        }
        catch (NotFoundException)
        {
            System.Console.WriteLine($"The {ideaRepoName} repository doesn't exist under the user: {githubUserName}");
            return false;
        }
        catch(Exception ex)
        {
            Console.WriteLine($"Error: {ex.Message}");
            return false;
        }
    }

}
