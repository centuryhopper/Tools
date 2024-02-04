using System.Diagnostics;
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

void print(object msg) => System.Console.WriteLine(msg);

var proxyServer = ProxySharp.Proxy.GetSingleProxy();
ProxySharp.Proxy.PopProxy();

DotNetEnv.Env.Load("secrets.env");

var emailSender = Environment.GetEnvironmentVariable("smtp_client")?.Split("|");
var receivers = Environment.GetEnvironmentVariable("smtp_receivers");
var isRunningInDocker = Environment.GetEnvironmentVariable("DOTNET_RUNNING_IN_CONTAINER");
var targetDir = Convert.ToBoolean(isRunningInDocker)
    ? "/app/amazon_products_outputs"
    : "./amazon_products_outputs";
Directory.CreateDirectory(targetDir);

// 200.143.75.194:8080

FirefoxOptions firefoxOptions = new FirefoxOptions();
firefoxOptions.AddArgument("--proxy-server=" + proxyServer);

firefoxOptions.AddArgument("--disable-blink-features=AutomationControlled");
firefoxOptions.AddArgument("--ignore-ssl-errors=yes");
firefoxOptions.AddArgument("--ignore-certificate-errors");
firefoxOptions.AddArgument("--disable-infobars");
firefoxOptions.AddArgument("--disable-popup-blocking");
firefoxOptions.AddArgument("--disable-extensions");

firefoxOptions.AddArgument("--headless");

// first tuple item should be a unique value
(string, string, string)[] queries =
{
    (
        "rove-r2-4k-pro-dashcam",
        "B0BZRKRBHP",
        "ROVE R2-4K PRO Dash Cam, Built-in GPS, 5G WiFi Dash Camera for Cars, 2160P UHD 30fps Dashcam with APP, 2.4\" IPS Screen, Night Vision, WDR, 150° Wide Angle, 24-Hr Parking Mode, Supports 512GB Max"
    ),
    ("wilson-evo-basketball-29_5", "wilson_evolution", "WILSON Evolution Indoor Game Basketballs - Size 5, Size 6 and Size 7")
};

ProductRecord? Extract(
    HtmlNode? node,
    int rowNumber,
    string titleDescription
)
{
    // find all the anchor nodes within each search section
    var anchorNodes = node?.SelectNodes(".//a");
    if (anchorNodes is null || !anchorNodes.Any())
        return null;

    string desc = "", url = "";
    foreach (var anchorNode in anchorNodes)
    {
        string href = anchorNode.GetAttributeValue("href", "");
        desc = anchorNode.InnerText.Trim();
        if (!desc.Contains(titleDescription))
        {
            continue;
        }
        url = $"https://www.amazon.com{href}";
    }

    if (string.IsNullOrEmpty(url))
    {
        return null;
    }

    // Console.WriteLine("Anchor Href: " + href);
    // Console.WriteLine("Anchor Content: " + desc);

    var priceParent = node?.SelectSingleNode(".//span[@class='a-price']");
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
    TimeSpan diff = DateTime.Now - lastDateTime;
    double diffInHours = diff.TotalSeconds / 3600;
    bool isConnectedToWifi = await IsConnectedToWifi();

    if (diffInHours < 24 || !isConnectedToWifi)
    {
        return;
    }

    var (filename, asin, desc) = query;
    List<ProductRecord> records = new();

    // Initialize FirefoxDriver
    using IWebDriver driver = new FirefoxDriver(firefoxOptions);

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
                    .Contains(
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
            }
        }
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
            case string s when s.Contains("ROVE R2-4K PRO Dash Cam", StringComparison.OrdinalIgnoreCase):
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
            case string s when s.Contains("WILSON Evolution Indoor Game Basketballs", StringComparison.OrdinalIgnoreCase):
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

string? ReadLastDateTimeFromTxt()
{
    string filePath = $"{Environment.CurrentDirectory}/script_execution_records/time_stamp.txt";
    if (File.Exists(filePath))
    {
        return File.ReadAllText(filePath);
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

var timeStampPath = $"{Environment.CurrentDirectory}/script_execution_records/";

Directory.CreateDirectory(timeStampPath);

var fullTimeStampPath = Path.Combine(timeStampPath, "time_stamp.txt");

using var sw = new StreamWriter(path: fullTimeStampPath, append: false);

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
