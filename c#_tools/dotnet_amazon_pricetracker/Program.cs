using System.Reflection;
using HtmlAgilityPack;
using OpenQA.Selenium;
using OpenQA.Selenium.Firefox;


void print(object msg) => System.Console.WriteLine(msg);

string assemblyPath = Assembly.GetExecutingAssembly().Location;
string programPath = Path.GetDirectoryName(assemblyPath)!;

Directory.SetCurrentDirectory(programPath);

// using StreamWriter writer = new StreamWriter(path: fullPath, append: false);
    
// // Write to the file
// writer.WriteLine(DateTime.Now.ToString(fmt));

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

// Initialize FirefoxDriver
using IWebDriver driver = new FirefoxDriver(firefoxOptions);

// Navigate to the webpage
driver.Navigate().GoToUrl("https://www.amazon.com/s?k=B07SXMZLPK&ref=nb_sb_noss_2");

// Get the page source
string pageSource = driver.PageSource;

// print(pageSource);

var doc = new HtmlDocument();
doc.LoadHtml(pageSource);

var results = doc.DocumentNode.Descendants("div")
    .Where(div => div.GetAttributeValue("class", "").Contains("a-section a-spacing-small puis-padding-left-small puis-padding-right-small"))
    .ToList();

foreach (var result in results)
{
    // string content = result.InnerText;
    // Console.WriteLine(content);
    var record = extract(result);
}

// TODO: extract for first 6 pages

(string, string, string, string, string, string)? extract(HtmlNode? node)
{
    var anchorNode = node?.SelectSingleNode(".//a");
    if (anchorNode is null)
        return null;
    
    string href = anchorNode.GetAttributeValue("href", "");
    string desc = anchorNode.InnerText.Trim();
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


    return (DateTime.Now.ToString("yyyy-MM-dd"), desc, priceText, ratingText, numReviewText, url);
}

// print(Directory.GetCurrentDirectory());

// var parallelOptions = new ParallelOptions { MaxDegreeOfParallelism = 6 };


// await Parallel.ForEachAsync(categories, parallelOptions, async (category, _) =>
// {
//     results.Add(await GetNews(category));
// });

// var outputPath = "news";
// var fileName = DateTime.Now.ToString("yyyy_MM_dd") + ".csv";
// Directory.CreateDirectory(outputPath);

// var fileCompletePath = Path.Combine(outputPath, fileName);