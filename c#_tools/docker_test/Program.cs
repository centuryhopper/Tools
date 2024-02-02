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

Console.WriteLine($"{DateTime.UtcNow}: Output String: '{secret}'");

Directory.SetCurrentDirectory(Directory.GetCurrentDirectory());
Directory.CreateDirectory("./test");

File.WriteAllText("./test/hello.txt", $"{DateTime.UtcNow}: Output String: '{secret}'");
