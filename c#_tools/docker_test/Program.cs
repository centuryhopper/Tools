using System;
using System.IO;
using docker_test;
using DotNetEnv;
using Microsoft.Extensions.Configuration;

DotNetEnv.Env.Load("secrets.env");

//var configuration = new ConfigurationBuilder()
//    .SetBasePath(Directory.GetCurrentDirectory())
//    .AddJsonFile("appsettings.json", optional: true, reloadOnChange: true)
//    .AddEnvironmentVariables(prefix: "CRON_")
//    .Build();
//Console.WriteLine(configuration["mySecret"]);

//var consoleSettings = new ConsoleSettings();
//configuration.GetSection("Console").Bind(consoleSettings);
var secret = Environment.GetEnvironmentVariable("mySecret");
var isRunningInDocker = Environment.GetEnvironmentVariable("DOTNET_RUNNING_IN_CONTAINER");

Console.WriteLine($"{DateTime.UtcNow}: Output String: '{secret}'");

//Console.WriteLine(Directory.GetCurrentDirectory());
var targetDir = Convert.ToBoolean(isRunningInDocker) ? "/app/target_dir" : "./target_dir";
Directory.SetCurrentDirectory(Directory.GetCurrentDirectory());

Directory.CreateDirectory(targetDir);

File.AppendAllText(
    Path.Combine(targetDir, "hello.txt"),
    $"{DateTime.UtcNow}: Output String: '{secret}'\n"
);
