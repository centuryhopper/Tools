using System.Reflection;
using dotnet_news_scraper.Models;
using Microsoft.Extensions.Configuration;
using NewsAPI;
using NewsAPI.Constants;
using NewsAPI.Models;
using OfficeOpenXml;

DotNetEnv.Env.Load("secrets.env");

var apiKey = Environment.GetEnvironmentVariable("apiKey");
var isRunningInDocker = Environment.GetEnvironmentVariable("DOTNET_RUNNING_IN_CONTAINER");
var targetDir = Convert.ToBoolean(isRunningInDocker) ? "/app/news" : "./news";
Directory.CreateDirectory(targetDir);

Categories[] categories =
{
    Categories.Business,
    Categories.Entertainment,
    Categories.Health,
    Categories.Science,
    Categories.Sports,
    Categories.Technology
};

async Task<(Categories, List<NewsArticleModel>)> GetNews(Categories category)
{
    var newsApiClient = new NewsApiClient(apiKey);
    var articlesResponse = await newsApiClient.GetTopHeadlinesAsync(
        new TopHeadlinesRequest
        {
            Language = Languages.EN,
            Category = category,
            Country = Countries.US
        }
    );
    List<NewsArticleModel> news = new();

    if (articlesResponse.Status == Statuses.Ok)
    {
        // total results found
        // Console.WriteLine(articlesResponse.TotalResults);

        // gets the first 20 by default

        foreach (var article in articlesResponse.Articles)
        {
            news.Add(
                new()
                {
                    Title = article.Title,
                    Author = article.Author,
                    Desc = article.Description,
                    Url = article.Url,
                    PublishedAt = article.PublishedAt!.Value.ToString("yyyy-MM-dd"),
                }
            );
        }
    }

    return (category, news);
}

var parallelOptions = new ParallelOptions { MaxDegreeOfParallelism = 6 };

List<(Categories, List<NewsArticleModel>)> results = new();

await Parallel.ForEachAsync(
    categories,
    parallelOptions,
    async (category, _) =>
    {
        results.Add(await GetNews(category));
    }
);

var outputPath = targetDir;
var fileName = DateTime.Now.ToString("yyyy_MM_dd") + ".xlsx";
var fileCompletePath = Path.Combine(outputPath, fileName);

//Console.WriteLine(fileCompletePath);
if (!File.Exists(fileCompletePath))
{
    // export to excel
    foreach (var (category, result) in results)
    {
        // print(result[0]);
        ExcelPackage.LicenseContext = LicenseContext.NonCommercial;
        using ExcelPackage pck = new(fileCompletePath);
        var ws = pck.Workbook.Worksheets.Add(category.ToString());
        var range = ws.Cells["A1"].LoadFromCollection(Collection: result, PrintHeaders: true);
        range.AutoFitColumns();

        await pck.SaveAsync();
    }
}
else
{
    Console.WriteLine($"{DateTime.Now.ToString("yyyy-MM-dd")}: Already retrieved today's news");
}
