﻿using System.Diagnostics;
using System.Drawing;
using System.Globalization;
using System.Reflection;
using CsvHelper;
using CsvHelper.Configuration;
using dotnet_amazon_pricetracker.Models;
using dotnet_amazon_pricetracker.Utils;
using HtmlAgilityPack;
using Microsoft.Extensions.Configuration;
using OpenQA.Selenium;
using OpenQA.Selenium.Firefox;

string fmt = "yyyy-MM-dd-HH:mm:ss";

void print(params object[] msg) => System.Console.WriteLine(string.Join(",", msg));

//var proxyServer = ProxySharp.Proxy.GetSingleProxy();
//ProxySharp.Proxy.PopProxy();

DotNetEnv.Env.Load("secrets.env");

var emailSender = Environment.GetEnvironmentVariable("smtp_client")?.Split("|");
var receivers = Environment.GetEnvironmentVariable("smtp_receivers");
var isRunningInDocker = Environment.GetEnvironmentVariable("DOTNET_RUNNING_IN_CONTAINER");
var targetDir = Convert.ToBoolean(isRunningInDocker)
    ? "/app/amazon_products_outputs"
    : "./amazon_products_outputs";
Directory.CreateDirectory(targetDir);

var timeStampDir = Convert.ToBoolean(isRunningInDocker)
    ? "/app/script_execution_records"
    : "./script_execution_records";
Directory.CreateDirectory(timeStampDir);

var geckoDriverPath = Convert.ToBoolean(isRunningInDocker) ? "/app/geckodriver" : "./geckodriver";

FirefoxOptions firefoxOptions = new FirefoxOptions();

//firefoxOptions.AddArgument("--proxy-server=" + proxyServer);

//firefoxOptions.AddArgument("--disable-blink-features=AutomationControlled");
//firefoxOptions.AddArgument("--ignore-ssl-errors=yes");
//firefoxOptions.AddArgument("--ignore-certificate-errors");
//firefoxOptions.AddArgument("--disable-infobars");
//firefoxOptions.AddArgument("--disable-popup-blocking");
//firefoxOptions.AddArgument("--disable-extensions");

firefoxOptions.SetPreference("webdriver.gecko.driver", geckoDriverPath);
firefoxOptions.BrowserExecutableLocation = "/usr/bin/firefox";
firefoxOptions.AddArgument("--headless");

// first tuple item should be a unique value
(string, string, string)[] queries =
{
    ("rove-r2-4k-pro-dashcam", "B0BZRKRBHP", "ROVE R2-4K PRO Dash Cam"),
    ("wilson-evo-basketball-29_5", "B00KXVPN8A", "Evolution Indoor Game Basketballs")
};

// inclusive of both bounds
float RandomRange(float min, float max)
{
    max += float.Epsilon;
    return min + (float)new Random().NextDouble() * (max - min);
}

ProductRecord? Extract(HtmlNode? node, int rowNumber, string titleDescription)
{
    // find all the anchor nodes within each search section
    var titleSection = node?.SelectSingleNode(".//div[@data-cy='title-recipe']");

    string desc = "";
    string url = "";

    var anchor = titleSection?.SelectSingleNode(
        ".//a[@class='a-link-normal s-underline-text s-underline-link-text s-link-style a-text-normal']"
    );

    if (anchor is null)
    {
        return null;
    }

    var hrefLink = anchor.GetAttributeValue("href", "");
    desc = titleSection?.InnerText.Trim() ?? "";
    if (desc.Contains(titleDescription))
    {
        url = $"https://www.amazon.com{hrefLink}";
        // if url is found, then we stop searching for more
    }

    if (string.IsNullOrEmpty(url))
    {
        return null;
    }

    // Console.WriteLine("Anchor Href: " + href);
    // Console.WriteLine("Anchor Content: " + desc);

    var priceSection = node?.SelectSingleNode(".//div[@data-cy='price-recipe']");
    var priceParent = priceSection?.SelectSingleNode(".//span[@class='a-price']");
    var price = priceParent?.SelectSingleNode(".//span[@class='a-offscreen']");

    if (price is null)
        return null;

    string priceText = price.InnerText.Trim();
    // Console.WriteLine("Price: " + priceText);

    var rating = node?.SelectSingleNode(".//i");

    string ratingText = rating is null ? "" : rating.InnerText.Trim();
    // print($"rating: {ratingText}");

    var numReview = node?.SelectSingleNode(".//span[@class='a-size-base s-underline-text']");

    string numReviewText = numReview is null ? "" : numReview.InnerText.Trim();
    // print($"numReview: {numReviewText}");

    return new ProductRecord(
        rowNumber,
        DateTime.Now.ToString("yyyy-MM-dd"),
        titleDescription,
        priceText,
        ratingText,
        numReviewText,
        url
    );
}

string GetFormattedUrl(string asin, int pageNumber)
{
    string url = $"https://www.amazon.com/s?k={asin}&page={pageNumber}";
    return url;
}

async Task ProcessQuery((string, string, string) query)
{
    string? lastReadDateTime = ReadLastDateTimeFromTxt();

    if (string.IsNullOrEmpty(lastReadDateTime))
    {
        // Epoch start time so that we're guaranteed to run the program when the
        // file is created Since it has definitely been at 24 hours since 1970
        lastReadDateTime = "1970-01-01-00:00:00";
    }

    DateTime lastDateTime = DateTime.ParseExact(lastReadDateTime.Trim(), fmt, null);

    if (lastDateTime.Date == DateTime.Today)
    {
        //print(lastDateTime.Date, DateTime.Today);
        return;
    }

    var (filename, asin, desc) = query;
    List<ProductRecord> records = new();

    // Initialize FirefoxDriver
    using IWebDriver driver = new FirefoxDriver(firefoxOptions);
    driver.Manage().Window.Size = new Size(1920, 1080);

    var completeFilePath = Path.Combine(targetDir, filename) + ".csv";

    // extract for first 6 pages
    for (int pageNumber = 1; pageNumber < 7; pageNumber++)
    {
        // Navigate to the webpage
        driver.Navigate().GoToUrl(GetFormattedUrl(asin, pageNumber));

        // Get the page source
        string pageSource = driver.PageSource;

        // print(pageSource);

        var doc = new HtmlDocument();
        doc.LoadHtml(pageSource);

        var searchSections = doc
            .DocumentNode.Descendants("div")
            .Where(div =>
                div.GetAttributeValue("class", "")
                    .Equals(
                        "a-section a-spacing-small puis-padding-left-small puis-padding-right-small"
                    )
            );

        foreach (var searchSection in searchSections)
        {
            var rowNumber = CountRowsInCsv(completeFilePath) + 1;
            var record = Extract(searchSection, rowNumber, desc);
            if (record is not null)
            {
                records.Add(record);
                // first one is probably it
                break;
            }
        }

        Thread.Sleep(TimeSpan.FromSeconds(RandomRange(0.1f, 1f)));
    }

    bool hasCsvAlready = Path.Exists(completeFilePath);

    var config = new CsvConfiguration(CultureInfo.InvariantCulture)
    {
        MemberTypes = CsvHelper.Configuration.MemberTypes.Fields,
        // Don't write the header again if file already exists
        HasHeaderRecord = hasCsvAlready ? false : true,
    };

    using var writer = new StreamWriter(completeFilePath, append: true);

    using var csv = new CsvWriter(writer, config);
    csv.Context.RegisterClassMap<ValueTupleMap>();
    csv.WriteRecords(records);

    AlertMeOfPriceDrops(records);
}

void AlertMeOfPriceDrops(List<ProductRecord> records)
{
    foreach (var record in records)
    {
        switch (record.Description)
        {
            case string s
                when s.Contains("ROVE R2-4K PRO Dash Cam", StringComparison.OrdinalIgnoreCase):
                if (float.Parse(record.PriceText.Replace("$", "")) < 100f)
                {
                    Helpers.SendEmail(
                        subject: "amazon rove dash cam",
                        body: $"the dash cam price dropped below $100!",
                        emailSender![0],
                        emailSender[1],
                        receivers: [receivers!]
                    );
                }
                break;
            case string s
                when s.Contains(
                    "WILSON Evolution Indoor Game Basketballs",
                    StringComparison.OrdinalIgnoreCase
                ):
                if (float.Parse(record.PriceText.Replace("$", "")) < 70f)
                {
                    Helpers.SendEmail(
                        subject: "wilson evolution indoor basketball",
                        body: $"the wilson basketball dropped below $70!",
                        emailSender![0],
                        emailSender[1],
                        receivers: [receivers!]
                    );
                }
                break;
            default:
                break;
        }
    }
}

string? ReadLastDateTimeFromTxt()
{
    string timeStampPath = Path.Combine(timeStampDir, "time_stamp.txt");
    if (File.Exists(timeStampPath))
    {
        return File.ReadAllText(timeStampPath);
    }

    return null;
}

int CountRowsInCsv(string csvPath)
{
    if (!Path.Exists(csvPath))
    {
        return 0;
    }
    // Read all lines from the CSV file into an array of strings
    string[] lines = File.ReadAllLines(csvPath);

    // Subtract 1 to exclude the header line
    int rowCount = lines.Length - 1;

    // Return the total row count
    return rowCount;
}

var parallelOptions = new ParallelOptions { MaxDegreeOfParallelism = 6 };

await Parallel.ForEachAsync(
    queries,
    parallelOptions,
    async (query, _) =>
    {
        await ProcessQuery(query: query);
    }
);

var timeStampPath = Path.Combine(timeStampDir, "time_stamp.txt");

using var sw = new StreamWriter(path: timeStampPath, append: false);

// mark time stamp to avoid re-running on the same day
sw.WriteLine(DateTime.Now.ToString(fmt));

public class ValueTupleMap
    : ClassMap<ValueTuple<int, string, string, string, string, string, string>>
{
    public ValueTupleMap()
    {
        Map(m => m.Item1).Name("Row #");
        Map(m => m.Item2).Name("Date");
        Map(m => m.Item3).Name("Description");
        Map(m => m.Item4).Name("Price");
        Map(m => m.Item5).Name("Rating");
        Map(m => m.Item6).Name("Review Count");
        Map(m => m.Item7).Name("URL");
    }
}
