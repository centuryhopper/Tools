using dotnet_news_scraper.Models;
using Microsoft.Extensions.Configuration;
using NewsAPI;
using NewsAPI.Constants;
using NewsAPI.Models;
using Newtonsoft.Json;
using OfficeOpenXml;

void print(object? val=null) => Console.WriteLine(val);

var builder = new ConfigurationBuilder()
    .AddUserSecrets<Program>();

var config = builder.Build();
var apiKey = config["apiKey"]!;
var category = "general";

string[] categories = {"business", "entertainment",
                          "general", "health", "science", "sports", "technology"};

async Task GetNews(string category)
{
    var newsApiClient = new NewsApiClient(apiKey);
    var articlesResponse = await newsApiClient.GetTopHeadlinesAsync(new TopHeadlinesRequest
    {
        Language = Languages.EN,
        Category = Categories.Business,
        Country = Countries.US
    });

    if (articlesResponse.Status == Statuses.Ok)
    {
        // total results found
        Console.WriteLine(articlesResponse.TotalResults);

        // gets the first 20 by default
        List<NewsArticleModel> news = new();

        foreach (var article in articlesResponse.Articles)
        {
            news.Add(
                new() {
                    Title = article.Title,
                    Author = article.Author,
                    Desc = article.Description,
                    Url = article.Url,
                    PublishedAt = article.PublishedAt!.Value.ToString("yyyy-MM-dd"),
                }
            );
        }

        // export to excel
        ExcelPackage.LicenseContext = LicenseContext.NonCommercial;

        using ExcelPackage pck = new();
        var ws = pck.Workbook.Worksheets.Add(category);

        var range = ws.Cells["A1"].LoadFromCollection(Collection: news, PrintHeaders: true);
        range.AutoFitColumns();

        await pck.SaveAsAsync(new FileInfo("news_test.xlsx"));
    }


}

await GetNews(category);


// TODO: make this multithreaded



