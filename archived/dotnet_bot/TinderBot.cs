using System.Drawing;
using System.Runtime.InteropServices;
using dotnet_bot.Abstract;
using OpenQA.Selenium;
using OpenQA.Selenium.Chrome;
using OpenQA.Selenium.Firefox;
using OpenQA.Selenium.Interactions;
using OpenQA.Selenium.Support.UI;
using WebDriverManager;
using WebDriverManager.DriverConfigs.Impl;

namespace dotnet_bot.Bot;

public class TinderBot : BotAbstract
{
    public IWebDriver driver { get; set; }
    public string username { get; set; }
    public string password { get; set; }
    public string name { get; set; }
    public string url { get; set; }
    public string screenshotsPath { get; set; }
    public const int MAX_NUM_FILES = 20;
    private Random random = new();
    public string photoFilePath { get; set; }

    public TinderBot(
        string username,
        string password,
        string uniqueName,
        string configsPath,
        string url = "https://tinder.com"
    )
    {
        // ProxySharp.Proxy.RenewQueue();
        //var proxyServer = ProxySharp.Proxy.GetSingleProxy();
        //ProxySharp.Proxy.PopProxy();
        this.username = username;
        this.password = password;
        this.name = uniqueName;
        this.url = url;
        photoFilePath = Path.Combine(configsPath, uniqueName, $"{uniqueName}_pics");
        screenshotsPath = Path.Combine(configsPath, uniqueName, "screenshots");
        // creates the path if it doesn't exist already
        Directory.CreateDirectory(screenshotsPath);
        driver = InitFirefoxDriver();
        driver.Navigate().GoToUrl(url);
    }

    private IWebDriver InitFirefoxDriver()
    {
        FirefoxOptions firefoxOptions = new FirefoxOptions
        {
            BrowserExecutableLocation = "/usr/bin/firefox"
        };
        // firefoxOptions.SetPreference("webdriver.gecko.driver", geckoDriverPath);

        firefoxOptions.SetPreference("geo.prompt.testing", true);
        firefoxOptions.SetPreference("geo.prompt.testing.allow", true);
        firefoxOptions.SetPreference(
            "geo.provider.network.url",
            "data:application/json,{\"location\": {\"lat\": 28.538336, \"lng\": -81.379234}, \"accuracy\": 100.0}"
        );

        // FirefoxDriverService service = FirefoxDriverService.CreateDefaultService();

        // Set the path of the GeckoDriver executable
        // service.FirefoxBinaryPath = geckoDriverPath;
        // new DriverManager().SetUpDriver(new FirefoxConfig());
        // IWebDriver driver = new FirefoxDriver(firefoxOptions);
        IWebDriver driver = new FirefoxDriver(firefoxOptions);
        driver.Manage().Window.Size = new Size(1920, 1080);
        return driver;
    }

    private void print(params object[] vals) => Console.WriteLine(string.Join(",", vals));

    public override IWebElement? WaitAndGet(string xpath, float seconds = 2.5f)
    {
        try
        {
            WebDriverWait wait = new WebDriverWait(driver, TimeSpan.FromSeconds(seconds));
            var element = wait.Until(condition =>
            {
                try
                {
                    var elementToBeDisplayed = driver.FindElement(By.XPath(xpath));
                    return elementToBeDisplayed.Displayed;
                }
                catch (StaleElementReferenceException)
                {
                    return false;
                }
                catch (NoSuchElementException)
                {
                    return false;
                }
                catch (Exception)
                {
                    return false;
                }
            });

            return element ? driver.FindElement(By.XPath(xpath)) : null;
        }
        catch (Exception e)
        {
            print(e.Message);
            return null;
        }
    }

    public override IWebElement[]? WaitAndGetAll(string xpath, float seconds = 2.5f)
    {
        try
        {
            WebDriverWait wait = new WebDriverWait(driver, TimeSpan.FromSeconds(seconds));
            var element = wait.Until(condition =>
            {
                try
                {
                    var elementToBeDisplayed = driver.FindElements(By.XPath(xpath));
                    return elementToBeDisplayed.All(e => e.Displayed);
                }
                catch (StaleElementReferenceException)
                {
                    return false;
                }
                catch (NoSuchElementException)
                {
                    return false;
                }
                catch (Exception)
                {
                    return false;
                }
            });

            return element ? driver.FindElements(By.XPath(xpath)).ToArray() : null;
        }
        catch (Exception e)
        {
            print(e.Message);
            return null;
        }
    }

    public override void HandlePopups(int milliseconds = 0)
    {
        Thread.Sleep(milliseconds);

        ClickButtonWithText("Allow");

        ClickButtonWithText("Not interested");

        ClickButtonWithText("miss out");

        // handle cookie declining
        try
        {
            var declineCookieButton = WaitAndGet("//div[contains(text(), 'I decline')]");
            declineCookieButton = GetButtonParent(declineCookieButton);
            declineCookieButton?.Click();
        }
        catch (System.Exception ex)
        {
            System.Console.WriteLine(ex.Message);
        }

        ClickButtonWithText("not interested", "no thanks");

        // maybe later popup
        ClickButtonWithText("maybe later");

        // last fallback
        PressEscape();
    }

    private void PressEscape()
    {
        try
        {
            // Create an instance of Actions class
            Actions actions = new Actions(driver);

            // Send the ESCAPE key
            actions.SendKeys(Keys.Escape).Perform();
        }
        catch (System.Exception) { }
    }

    private List<string> GetMatches()
    {
        var matchProfiles = WaitAndGetAll("//a[contains(@class, 'matchListItem')]");
        var messageLinks = new List<string>();
        foreach (var profile in matchProfiles ?? [])
        {
            if (
                profile.GetAttribute("href") == "https://tinder.com/app/my-likes"
                || profile.GetAttribute("href") == "https://tinder.com/app/likes-you"
            )
            {
                continue;
            }

            messageLinks.Add(profile.GetAttribute("href"));
        }

        return messageLinks;
    }

    public override void SendMessageToMatches()
    {
        var links = GetMatches();
        foreach (var link in links)
        {
            SendMessage(link);
            Thread.Sleep(random.Next(100, 500));
        }
    }

    private void SendMessage(string link)
    {
        driver.Navigate().GoToUrl(link);
        Thread.Sleep(random.Next(4000, 5000));
        var textArea = driver.FindElement(By.TagName("textarea"));
        textArea.Clear();
        var name = driver.FindElement(By.TagName("h1"))?.Text;
        textArea.SendKeys($"nice to meet you {name ?? ""}. Hru? Got snap and/or insta? ");
        //textArea.SendKeys($"Happy new year!");

        var span = WaitAndGet("//span[contains(text(), 'Send')]");
        span = GetButtonParent(span);
        span?.Click();
    }

    public override void Screenshot()
    {
        try
        {
            string screenshotFileName = Path.Combine(
                screenshotsPath,
                $"{DateTime.Now:yyyy_MM_dd_HH_mm_ss}.png"
            );
            ((ITakesScreenshot)driver)
                .GetScreenshot()
                .SaveAsFile(screenshotFileName, ScreenshotImageFormat.Png);

            if (Directory.GetFiles(screenshotsPath).Length > MAX_NUM_FILES)
            {
                foreach (var file in Directory.GetFiles(screenshotsPath))
                {
                    File.Delete(Path.Combine(screenshotsPath, file));
                }
            }
        }
        catch
        {
            // Handle any exception
        }
    }

    public override bool TryLogin()
    {
        // count the number of attempts to login
        int tries = 0;

        bool couldLogin = GetButtonParent(WaitAndGet("//div[text()='Create account']")) is null;

        do
        {
            tries++;

            Screenshot();
            var buttons = driver.FindElements(By.TagName("button"));
            foreach (var button in buttons)
            {
                if (button is not null && button.Text.ToLower().Contains("i decline"))
                {
                    button.Click();
                    break;
                }
            }

            try
            {
                var googleAccountsPopupClose = driver.FindElement(By.Id("close"));
                googleAccountsPopupClose?.Click();
            }
            catch (System.Exception)
            {
            }

            try
            {

                // press login button
                var loginButton = WaitAndGet(
                    "//a[contains(@href,'https://tinder.onelink.me/9K8a/3d4abb81')]",
                    5
                );
                loginButton?.Click();

                var loginWithFacebook = WaitAndGet(
                    "//button[contains(@aria-label,'Log in with Facebook')]",
                    2.5f
                );
                loginWithFacebook?.Click();

                bool canContinue = false;
                var baseWindow = driver.WindowHandles[0];
                while (!canContinue)
                {
                    try
                    {
                        driver.SwitchTo().Window(driver.WindowHandles[1]);
                        canContinue = true;
                    }
                    catch (Exception)
                    {
                        canContinue = false;
                    }
                }
                // sign in
                var usernameField = WaitAndGet("//*[@id='email']", 2.5f);
                var passwordField = WaitAndGet("//*[@id='pass']", 2.5f);

                usernameField?.SendKeys(username);
                passwordField?.SendKeys(password);

                Screenshot();

                // log in
                var facebookLoginButton = WaitAndGet("//input[starts-with(@id, 'u_0_0_')]", 2.5f);
                facebookLoginButton?.Click();

                var continueAsButton = WaitAndGet("//span[contains(translate(text(), 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz'), 'continue as')]");
                continueAsButton?.Click();

                // switch back to main window handle
                driver.SwitchTo().Window(baseWindow);

                Thread.Sleep(random.Next(5000, 10000));

                Screenshot();
            }
            catch (System.Exception)
            {

            }

            Thread.Sleep(random.Next(5000, 10000));

            couldLogin = GetButtonParent(WaitAndGet("//div[text()='Create account']")) is null;
            // System.Console.WriteLine("couldlogin: " + couldLogin);
            // System.Console.WriteLine(tries);
            if (couldLogin)
            {
                break;
            }
        }
        while (tries < 3);

        if (!couldLogin)
        {
            // System.Console.WriteLine("unable to login for " + name);
            return false;
        }

        // System.Console.WriteLine("handling pop ups rn");

        HandlePopups(random.Next(2000, 4000));

        return true;
    }

    public override bool Like()
    {
        Thread.Sleep(random.Next(500, 1500));
        try
        {
            Screenshot();
            var likeSpan = WaitAndGet("//span[text()='Like']");
            likeSpan = GetButtonParent(likeSpan);
            likeSpan?.Click();

            return true;
        }
        catch (Exception ex)
        {
            return false;
        }
    }

    public override bool NoMoreLikes()
    {
        var outOfLikes = WaitAndGet("//h3[text()='Select a plan']");

        outOfLikes ??= WaitAndGet("//h2[text()='Unlimited Likes. Send as many likes as you want.']");

        return outOfLikes is not null;
    }

    public override bool DisLike()
    {
        try
        {
            var dislikeSpan = WaitAndGet("//span[text()='Nope']");
            dislikeSpan = GetButtonParent(dislikeSpan);
            dislikeSpan?.Click();
            return true;
        }
        catch (Exception ex) {
            return false;
        }
    }

    public override bool RanOutOfMatches()
    {
        try
        {
            var outOfMatchesNotice = driver.FindElement(
                By.XPath(
                    $"//div[contains(text(),'We are unable to find any potential matches right now. Try changing your preferences to see who is nearby')]"
                )
            );
            return true;
        }
        catch (Exception ex)
        {
            return false;
        }
    }

    public override void AutoSwipe()
    {
        while (true)
        {
            //print("hello", "from", "autoswipe");
            if (!Like())
            {
                // check for no more likes pop up
                if (NoMoreLikes())
                {
                    // print("finished!");
                    break;
                }

                // check whether we ran out of matches
                if (RanOutOfMatches())
                {
                    // refresh the page
                    driver.Navigate().Refresh();
                }

                // since we still have likes left, this could have been a random popup
                PressEscape();

                HandlePopups();
            }
        }
    }

    public override IWebElement? GetButtonParent(IWebElement? e)
    {
        return base.GetButtonParent(e);
    }

    private void ClickButtonWithText(params string[] texts)
    {
        Screenshot();
        try
        {
            var buttons = driver.FindElements(By.TagName("button"));
            foreach (var button in buttons)
            {
                if (button is not null && texts.Any(text => button.Text.ToLower().Contains(text.ToLower())))
                {
                    button?.Click();
                    break;
                }
            }
        }
        catch (ElementNotInteractableException e) { }
        catch (StaleElementReferenceException e) { }
        catch (Exception ex)
        {
            System.Console.WriteLine(ex.Message);
        }
    }

    public override void Dispose()
    {
        driver.Quit();
    }

}
