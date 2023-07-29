using System.Diagnostics;
using System.Globalization;
using System.Reflection;
using CsvHelper;
using CsvHelper.Configuration;
using HtmlAgilityPack;
using OpenQA.Selenium;
using OpenQA.Selenium.Firefox;


string currentDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)!;
string parentDirectory = Directory.GetParent(currentDirectory)!.FullName;
Directory.SetCurrentDirectory(parentDirectory);
string fmt = "yyyy-MM-dd-HH:mm:ss";


void print(object msg) => System.Console.WriteLine(msg);


var proxyServer = ProxySharp.Proxy.GetSingleProxy();


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
(string, string, string)[] queries = {("ryzen_7","B07SXMZLPK", "AMD Ryzen 7 3700X 8-Core, 16-Thread Unlocked Desktop Processor with Wraith Prism LED Cooler")};


(int, string, string, string, string, string, string)? Extract(HtmlNode? node, int rowNumber, string query)
{
    var anchorNode = node?.SelectSingleNode(".//a");
    if (anchorNode is null)
        return null;
    
    string href = anchorNode.GetAttributeValue("href", "");
    string desc = anchorNode.InnerText.Trim();
    if (!desc.Contains(query))
    {
        return null;
    }
    Console.WriteLine("Anchor Href: " + href);
    Console.WriteLine("Anchor Content: " + desc);

    string url = $"https://www.amazon.com{href}";
    
    var priceParent = node?.SelectSingleNode(".//span[@class='a-price']");
    var price = priceParent?.SelectSingleNode(".//span[@class='a-offscreen']");

    if (price is null)
        return null;
    
    string priceText = price.InnerText.Trim();
    Console.WriteLine("Price: " + priceText);

    var rating = node?.SelectSingleNode(".//i");

    string ratingText = rating is null ? "" : rating.InnerText.Trim();
    print($"rating: {ratingText}");

    var numReview = node?.SelectSingleNode(".//span[@class='a-size-base s-underline-text']");
    
    string numReviewText = numReview is null ? "" : numReview.InnerText.Trim();
    print($"numReview: {numReviewText}");

    return (rowNumber, DateTime.Now.ToString("yyyy-MM-dd"), desc, priceText, ratingText, numReviewText, url);
}

string GetFormattedUrl(string asin, int pageNumber)
{
    string url = $"https://www.amazon.com/s?k={asin}&ref=nb_sb_noss_2&page={pageNumber}";
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
    List<(int,string,string,string,string,string,string)> rows = new();

    // Initialize FirefoxDriver
    using IWebDriver driver = new FirefoxDriver(firefoxOptions);

    var recordsPath = $"{Environment.CurrentDirectory}/records/";

    Directory.CreateDirectory(recordsPath);

    recordsPath = Path.Combine(recordsPath, filename) + ".csv";

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

        var results = doc.DocumentNode.Descendants("div")
            .Where(div => div.GetAttributeValue("class", "").Contains("a-section a-spacing-small puis-padding-left-small puis-padding-right-small"));

        foreach (var result in results)
        {
            // string content = result.InnerText;
            // Console.WriteLine(content);
            var rowNumber = CountRowsInCsv(recordsPath) + 1;
            var record = Extract(result, rowNumber, desc);
            if (record is not null)
            {
                rows.Add(record.GetValueOrDefault());
            }
        }
    }

    bool hasCsvAlready = Path.Exists(recordsPath);
    
    var config = new CsvConfiguration(CultureInfo.InvariantCulture)
    {
        MemberTypes = CsvHelper.Configuration.MemberTypes.Fields,

        // Don't write the header again if file already exists
        HasHeaderRecord = hasCsvAlready ? false : true,
    };

    using var writer = new StreamWriter(recordsPath, append: true);

    using var csv = new CsvWriter(writer, config);
    csv.Context.RegisterClassMap<ValueTupleMap>();
    csv.WriteRecords(rows);
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

await Parallel.ForEachAsync(queries, parallelOptions, async (query, _) =>
{
    await ProcessQuery(query: query);
});

var timeStampPath = $"{Environment.CurrentDirectory}/script_execution_records/";

Directory.CreateDirectory(timeStampPath);

var fullTimeStampPath = Path.Combine(timeStampPath, "time_stamp.txt");

using var sw = new StreamWriter(path: fullTimeStampPath, append: false);

// mark time stamp to avoid re-running on the same day
sw.WriteLine(DateTime.Now.ToString(fmt));















public class ValueTupleMap : ClassMap<ValueTuple<int,string,string,string,string,string,string>>
{
    public ValueTupleMap()
    {
        Map(m => m.Item1).Name("Row #");
        Map(m => m.Item2).Name("Date");
        Map(m => m.Item3).Name("Description");
        Map(m => m.Item4).Name("Price");
        Map(m => m.Item5).Name("Rating'");
        Map(m => m.Item6).Name("Review Count");
        Map(m => m.Item7).Name("URL");
    }
}



