using System.Drawing;
using System.Runtime.InteropServices.ComTypes;
using System.Text.RegularExpressions;
using dotnet_bmbl_bot.Models;
using OpenQA.Selenium;
using OpenQA.Selenium.Firefox;
using OpenQA.Selenium.Interactions;
using OpenQA.Selenium.Support.UI;
using RandomUserAgent;
using WebDriverManager;
using WebDriverManager.DriverConfigs.Impl;

namespace dotnet_bmbl_bot.Bot;

public class BumbleBot : IDisposable
{
    public IWebDriver driver { get; set; }
    public string username { get; set; }
    public string password { get; set; }
    public string name { get; set; }
    public string url { get; set; }
    public string photoFilePath { get; set; }
    public AccountConfig Account { get; set; }
    public string screenshotsPath { get; set; }
    public bool isCreatingAccount { get; set; }

    public const int MAX_NUM_FILES = 20;
    private readonly string[] greets =
    {
        "Hey $, I'm all about adventure and trying new things. Got any exciting stories to share?",
        "Hi there! Do you have any hidden talents or secret passions that you're willing to reveal?",
        "Hello $, I've heard that a sense of humor can be pretty attractive. Care to share your favorite funny story?",
        "Hey, imagine we could plan the perfect date night together. What would it look like?",
        "Hi $, let's play a game of 'Would You Rather.' I'll start: Would you rather go on a spontaneous road trip or have a cozy movie night in?",
        "Hey $, what's your favorite way to unwind and let loose after a long day?",
        "Hi there! I'm all about making memories. Got any memorable experiences you'd like to share?",
        "Hello $, what's your idea of a perfect, fun-filled day?",
        "Hey $, I've got a question that might make you smile: What's the most exciting thing on your dating app bucket list?",
        "I dunno whats hotter, FL heat or you🥵$.",
        "I am looking to explore new sexual experiences with someone and to also someone to go out and travel, go to theme parks with. If that sounds good to you this holiday season, may i have your number?"
    };
    private readonly string geckodriverPath;
    private readonly string configsPath;

    // name map to age which maps to the profile bio
    private Dictionary<string, Dictionary<string, string>>? left_swipe_dict { get; set; }

    private Random random = new();
    private string latitude,
        longitude;

    public BumbleBot(
        string username,
        string password,
        string uniqueName,
        string geckodriverPath,
        string configsPath,
        string url = "https://bumble.com/get-started",
        Dictionary<string, AccountConfig>? accounts = null,
        Dictionary<string, Dictionary<string, string>>? left_swipe_dict = null,
        bool isCreatingAccount = false
    )
    {
        this.username = username;
        this.password = password;
        this.name = uniqueName;
        this.geckodriverPath = geckodriverPath;
        this.configsPath = configsPath;
        this.url = url;
        this.photoFilePath = Path.Combine(Environment.CurrentDirectory, $"{uniqueName}_pics");
        System.Console.WriteLine(uniqueName);
        Account = accounts![uniqueName];
        this.left_swipe_dict = left_swipe_dict;
        screenshotsPath = Path.Combine(Directory.GetCurrentDirectory(), uniqueName, "screenshots");
        screenshotsPath = Path.Combine(configsPath, uniqueName, "screenshots");

        this.isCreatingAccount = isCreatingAccount;
        this.latitude = Account.Latitude;
        this.longitude = Account.longitude;

        // creates the path if it doesn't exist already
        Directory.CreateDirectory(screenshotsPath);

        this.driver = InitDriver();
        print("log in");
        while (!TryLogin())
        {
            this.driver = InitDriver();
        }
    }

    private IWebDriver InitDriver()
    {
        // ProxySharp.Proxy.RenewQueue();
        // var proxyServer = ProxySharp.Proxy.GetSingleProxy();
        // ProxySharp.Proxy.PopProxy();

        FirefoxOptions firefoxOptions = new FirefoxOptions
        {
            BrowserExecutableLocation = "/usr/bin/firefox"
        };
        // firefoxOptions.AddArgument("--proxy-server=" + proxyServer);

        // firefoxOptions.AddArgument("--disable-blink-features=AutomationControlled");

        if (!this.isCreatingAccount)
        {
            //firefoxOptions.AddArgument("--headless");
        }

        // firefoxOptions.AddArgument("--incognito");
        // firefoxOptions.AddArgument("--disable-dev-shm-usage");
        // firefoxOptions.AddArgument("--ignore-ssl-errors=yes");
        // firefoxOptions.AddArgument("--ignore-certificate-errors");
        // firefoxOptions.AddArgument("--disable-infobars");
        // firefoxOptions.AddArgument("--disable-extensions");
        // firefoxOptions.AddArgument("--disable-popup-blocking");
        string userAgent = RandomUa.RandomUserAgent;
        // System.Console.WriteLine(userAgent);
        // firefoxOptions.AddArgument(
        //    $"--user-agent={userAgent}"
        // );

        // user agent
        //  firefoxOptions.AddArgument("--disable-notifications");
        // firefoxOptions.AddArgument("--disable-web-security");
        // firefoxOptions.AddArgument("--private");

        // firefoxOptions.AddArgument("--disable-setuid-sandbox");
        // firefoxOptions.AddArgument("--safebrowsing-disable-download-protection");

        //FirefoxDriverService service = FirefoxDriverService.CreateDefaultService();

        // Set the path of the GeckoDriver executable
        // service.FirefoxBinaryPath = geckodriverPath;

        firefoxOptions.SetPreference("geo.prompt.testing", true);
        firefoxOptions.SetPreference("geo.prompt.testing.allow", true);
        firefoxOptions.SetPreference(
            "geo.provider.network.url",
            "data:application/json,{\"location\": {\"lat\":"
                + this.latitude
                + ",\"lng\":"
                + this.longitude
                + "},\"accuracy\": 100.0}"
        );

        //new DriverManager().SetUpDriver(new FirefoxConfig());
        IWebDriver driver = new FirefoxDriver(firefoxOptions);
        driver.Manage().Window.Size = new Size(1920, 1080);
        return driver;
    }

    private void print(object val) => Console.WriteLine(val);

    private IWebElement? waitAndGet(string xpath, double seconds = 2.5)
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

    private IWebElement[]? waitAndGetAll(string xpath, double seconds = 5)
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
                catch (Exception e)
                {
                    print(e.Message);
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

    private void HandlePopups()
    {
        try
        {
            // var continueWithBumbleButton =
            // waitAndGet("//span[contains(text(),'Continue Bumbling')]");
            // continueWithBumbleButton = GetButtonParent(continueWithBumbleButton);
            // continueWithBumbleButton?.Click();

            var buttonLst = waitAndGetAll("//div[contains(@data-qa-role, 'button')]");
            foreach (var button in buttonLst)
            {
                if (button.Text.Contains("Continue Bumbling"))
                {
                    button.Click();
                    break;
                }
            }
        }
        catch (System.Exception) { }

        try
        {
            var deactivateButton = waitAndGet("//span[contains(text(), 'Deactivate')]");
            deactivateButton = GetButtonParent(deactivateButton);
            deactivateButton?.Click();
        }
        catch (System.Exception) { }

        // last fallback
        PressKey(Keys.Escape);
    }

    private void PressKey(string key)
    {
        try
        {
            // Create an instance of Actions class
            Actions actions = new Actions(driver);

            // Send the ESCAPE key
            actions.SendKeys(key).Perform();
        }
        catch (System.Exception) { }
    }

    public void SendMessageToMatches()
    {
        var lst = driver.FindElements(By.ClassName("contact"));
        foreach (var contact in lst)
        {
            contact.Click();
            var convo = waitAndGet("//*[starts-with(@class, 'messages-list__conversation')]");

            // get all child elements of convo
            var msgs = convo?.FindElements(By.XPath("./*"));
            if (msgs is null)
            {
                // expired match
                continue;
            }

            bool didIMessageBefore = msgs.Any(msg => msg.GetAttribute("class").EndsWith("out"));

            if (!didIMessageBefore)
            {
                IEnumerable<IWebElement> tas;
                try
                {
                    // send a greeting
                    tas = driver.FindElements(By.TagName("textarea")).AsEnumerable();
                }
                catch (Exception e)
                {
                    break;
                }

                var inputBox = tas.Where(e =>
                        e.GetAttribute("placeholder").ToLower().StartsWith("start chatting")
                    )
                    .FirstOrDefault();

                inputBox?.Clear();

                string? matchName = "";

                try
                {
                    var aside = driver.FindElement(By.ClassName("page__profile is-expanded"));
                    matchName = aside
                        .FindElement(By.ClassName("profile__name"))
                        ?.Text.ToLower()
                        .Trim();
                }
                catch (Exception e) { }

                var customMsg = greets[random.Next(greets.Length)].Replace("$", matchName);
                customMsg = "Hru? Got snap and/or insta?";
                inputBox?.SendKeys(customMsg);

                PressKey(Keys.Enter);
            }
        }
    }

    public void Screenshot(string? path = null)
    {
        try
        {
            if (string.IsNullOrEmpty(path))
            {
                string screenshotFileName = Path.Combine(
                    screenshotsPath,
                    $"{DateTime.Now.ToString("yyyy-MM-dd-HH_mm_ss")}.png"
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
            else
            {
                path = Path.Combine(Directory.GetCurrentDirectory(), name, path);
                Directory.CreateDirectory(path);
                string screenshotFileName = Path.Combine(
                    path,
                    $"{DateTime.Now.ToString("yyyy-MM-dd-HH_mm_ss")}.png"
                );
                ((ITakesScreenshot)driver)
                    .GetScreenshot()
                    .SaveAsFile(screenshotFileName, ScreenshotImageFormat.Png);

                if (Directory.GetFiles(path).Length > MAX_NUM_FILES)
                {
                    foreach (var file in Directory.GetFiles(path))
                    {
                        File.Delete(Path.Combine(path, file));
                    }
                }
            }
        }
        catch
        {
            // Handle any exception
        }
    }

    public bool TryLogin()
    {
        print("trying to log in...");
        driver.Navigate().GoToUrl(url);
        Thread.Sleep(random.Next(2000, 4000));
        Screenshot();
        var mainWindow = driver.WindowHandles[0];

        var elem = driver.FindElement(By.XPath("//iframe[@title='SP Consent Message']"));

        print(elem.Text);
        try
        {
            driver
                .SwitchTo()
                .Frame(driver.FindElement(By.XPath("//iframe[@title='SP Consent Message']")));
            var buttons = driver.FindElements(By.TagName("button"));
            foreach (var button in buttons)
            {
                if (
                    button is not null
                    && (button.GetAttribute("title").ToLower().Contains("continue") || button.GetAttribute("title").ToLower().Contains("don't agree"))
                )
                {
                    button.Click();
                    break;
                }
            }
        }
        catch (System.Exception ex)
        {
            print(ex);
        }

        // switch back from iframe to main window
        driver.SwitchTo().Window(mainWindow);

        // press facebook button
        var buttonLst = waitAndGetAll("//button[contains(@data-qa-role, 'button')]");

        if (buttonLst is null)
        {
            Dispose();
            return false;
        }

        foreach (var button in buttonLst)
        {
            if (button is not null && button.Text.ToLower().Contains("facebook"))
            {
                button.Click();
                break;
            }
        }

        Thread.Sleep(random.Next(2000, 4000));

        bool canContinue = false;
        // try switching to facebook sign in popup
        var baseWindow = driver.WindowHandles[0];
        int count = 0;
        while (!canContinue && count < 5)
        {
            try
            {
                driver.SwitchTo().Window(driver.WindowHandles[1]);
                canContinue = true;
            }
            catch (Exception)
            {
                canContinue = false;
                count++;
            }
        }

        if (count >= 5)
        {
            Dispose();
            return false;
        }

        // sign in
        var usernameField = waitAndGet("//*[@id='email']", 2.5);
        var passwordField = waitAndGet("//*[@id='pass']", 2.5);

        usernameField?.SendKeys(username);
        passwordField?.SendKeys(password);


        Screenshot();

        // log in
        var facebookLoginButton = waitAndGet("//input[starts-with(@id, 'u_0_0_')]", 2.5);
        facebookLoginButton?.Click();

        var continueAsButton = waitAndGet("//span[contains(translate(text(), 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz'), 'continue as')]");
        continueAsButton?.Click();

        // switch back to main window handle
        driver.SwitchTo().Window(baseWindow);

        Thread.Sleep(random.Next(2000, 4000));

        Screenshot();

        return true;
    }

    private bool Like()
    {
        try
        {
            Screenshot();
            Thread.Sleep(random.Next(500, 1500));
            var likeDiv = waitAndGet("//div[@data-qa-role='encounters-action-like']");
            likeDiv.Click();

            return true;
        }
        catch (Exception)
        {
            return false;
        }
    }

    private bool NoMoreLikes()
    {
        var buttonLst = waitAndGetAll("//button[contains(@data-qa-role, 'button')]");

        if (buttonLst is null)
        {
            return false;
        }

        foreach (var button in buttonLst!)
        {
            if (button is not null && button.Text.ToLower().Contains("upgrade for more people"))
            {
                return true;
            }
        }

        return false;
    }

    private void DisLike()
    {
        var dislikeDiv = waitAndGet("//div[contains(@data-qa-role,'encounters-action-dislike')]");
        dislikeDiv?.Click();
    }

    private bool ShouldSwipeLeft(string name, string age, string about)
    {
        if (left_swipe_dict.TryGetValue(name, out Dictionary<string, string> val))
        {
            if (val.TryGetValue(age, out string bio))
            {
                if (about.Contains(bio))
                {
                    return true;
                }
            }
        }

        return false;
    }

    private bool RanOutOfMatches()
    {
        bool retVal = false;
        try
        {
            var outOfMatchesNotice = driver.FindElement(
                By.XPath(
                    "//*[contains(text(),'You’re all caught up!')]"
                )
            );
            retVal |= outOfMatchesNotice is not null;
        }
        catch (Exception ex)
        {
            
        }

        retVal |= driver.FindElements(By.TagName("button")).Any(b=>b.Text.ToLower() == "back to homepage");

        return retVal;
    }


    public void AutoSwipe()
    {
        while (true)
        {
            // string profileName, profileAge, profileAbout;
            // try
            //{
            //     Thread.Sleep(TimeSpan.FromSeconds(randomRange(0.25f, 0.5f)));
            //     // Find the elements by class name
            //     IWebElement profileNameElement =
            //         driver.FindElement(By.ClassName("encounters-story-profile__name"));
            //     IWebElement profileAgeElement =
            //         driver.FindElement(By.ClassName("encounters-story-profile__age"));

            //    // Get the text and convert to lowercase
            //    profileName = profileNameElement.Text.ToLower();
            //    profileAge = profileAgeElement.Text.ToLower();

            //    Thread.Sleep(TimeSpan.FromSeconds(randomRange(0.25f, 0.5f)));

            //    PressKey(Keys.Down);

            //    Thread.Sleep(TimeSpan.FromSeconds(randomRange(0.25f, 0.5f)));

            //    IWebElement profileAboutElement = driver.FindElement(
            //        By.ClassName("encounters-album")
            //    );

            //    profileAbout = profileAboutElement.Text.ToLower();

            //    // Use regular expressions to extract letters and digits
            //    profileAge = string.Join("", Regex.Matches(profileAge, @"\d"));
            //    Console.WriteLine(string.Format("name: {0}", profileName));
            //    Console.WriteLine(string.Format("age: {0}", profileAge));
            //    Console.WriteLine(string.Format("about: {0}", profileAbout));
            //}
            // catch (Exception e)
            //{
            //    break;
            //}

            // if (ShouldSwipeLeft(profileName, profileAge, profileAbout))
            //{
            //     Screenshot("left_swipes");
            //     DisLike();
            // }
            // else if (!Like())
            //{
            //     // check for no more likes pop up
            //     if (NoMoreLikes())
            //     {
            //         break;
            //     }

            //    // since we still have likes left, this could have been a random
            //    popup HandlePopups();
            //}

            if (!Like())
            {
                // check for no more likes pop up
                if (NoMoreLikes())
                {
                    break;
                }

                if (RanOutOfMatches())
                {
                    driver.Navigate().Refresh();
                }

                // since we still have likes left, this could have been a random popup
                HandlePopups();
            }
        }
    }

    private IWebElement? GetButtonParent(IWebElement? e)
    {
        if (e is null)
            return null;

        var parent = e.FindElement(By.XPath(".."));
        if (parent.TagName.ToLower() == "button")
        {
            return e;
        }

        return GetButtonParent(parent);
    }

    // inclusive of both bounds
    private float randomRange(float min, float max)
    {
        max += float.Epsilon;
        return min + (float)random.NextDouble() * (max - min);
    }

    // account creation methods start here
    public void SignUp()
    {
        try
        {
            driver
                .SwitchTo()
                .Frame(driver.FindElement(By.XPath("//iframe[@title='SP Consent Message']")));

            var buttons = driver.FindElements(By.TagName("button"));
            foreach (var button in buttons)
            {
                if (
                    button is not null
                    && button.GetAttribute("title").ToLower().Contains("continue")
                )
                {
                    button.Click();
                    break;
                }
            }
        }
        catch (Exception e)
        {
            print(e.Message);
        }

        var bumbleWindow = driver.WindowHandles[0];
        driver.SwitchTo().Window(bumbleWindow);

        var possiblefbPath1 =
            "//*[@id='main']/div/div[1]/div[2]/main/div/div[3]/form/div[1]/div/div[1]/div";

        var possiblefbPath2 =
            "//*[@id='main']/div/div[1]/div[2]/main/div/div[3]/form/div[1]/div/div[2]/div";

        waitAndGet(possiblefbPath1)?.Click();
        waitAndGet(possiblefbPath2)?.Click();

        var popUpWindow = driver.WindowHandles[1];

        driver.SwitchTo().Window(popUpWindow);

        waitAndGet("//*[@id='email']")?.SendKeys(username);
    }

    public void InitialSetup()
    {
        var allowLocationAccessXpath =
            "//*[@id='main']/div/div[1]/div[2]/main/div/div[5]/form/div/div";

        waitAndGet(allowLocationAccessXpath)?.Click();

        var continueXpath = "//*[@id='main']/div/div[1]/div[2]/main/div/div[4]/form/div[3]/div";

        waitAndGet(continueXpath)?.Click();

        var iAgreeXpath = "//*[@id='main']/div/div[1]/div[1]/main/div/div[8]/div";

        waitAndGet(iAgreeXpath)?.Click();

        var continueWithDateXpath = "//*[@id='main']/div/div[1]/div[1]/main/section/div/div[3]/div";

        waitAndGet(continueWithDateXpath)?.Click();
    }

    public void InitialUploadPhotos()
    {
        try
        {
            var photos = Directory.GetFiles(this.photoFilePath);

            if (photos.Length > 6)
            {
                throw new Exception("You can only have up to 6 photos");
            }
            foreach (var photo in photos)
            {
                var labels = driver.FindElements(By.ClassName("file-upload__input"));
                labels[0]
                    .FindElement(By.TagName("input"))
                    .SendKeys($"{Path.Combine(photoFilePath, photo)}");
                Thread.Sleep(TimeSpan.FromSeconds(randomRange(2, 3)));
            }
        }
        catch (Exception e)
        {
            print(e.Message);
        }

        var finishPhotosXpath = "//span[text()='Continue']";
        waitAndGet(finishPhotosXpath)?.Click();
    }

    public void FilterAgeAndDistance()
    {
        Thread.Sleep(TimeSpan.FromSeconds(randomRange(2, 3)));

        try
        {
            var filterToggleClassName = "encounters-header__filter-toggle";
            driver.FindElement(By.ClassName(filterToggleClassName))?.Click();
        }
        catch (Exception e) { }

        Thread.Sleep(TimeSpan.FromSeconds(randomRange(1, 2)));

        try
        {
            var women = driver.FindElements(By.ClassName("switcher__option-content"))[0];
            women?.Click();
        }
        catch (Exception e) { }

        Thread.Sleep(TimeSpan.FromSeconds(randomRange(1, 2)));

        Actions actions = new(driver);

        // distance
        var scope = waitAndGet("//div[contains(@data-qa-role, 'distance-settings-slider')]");

        var divs = scope?.FindElements(By.TagName("div"));
        IWebElement? distanceSlider = null;

        foreach (var div in divs)
        {
            if (
                div is not null
                && div.GetAttribute("class")
                    .Contains("range-slider__track-mover range-slider__track-mover--to")
            )
            {
                distanceSlider = div;
            }
        }

        for (var i = 0; i < 2; i++)
        {
            actions.ClickAndHold(distanceSlider).MoveByOffset(100, 0).Release().Perform();
            Thread.Sleep(TimeSpan.FromSeconds(randomRange(3, 5)));
        }

        var apply = waitAndGet(
            "/html/body/div/div/div[1]/main/div[1]/div/div[1]/div[1]/div/div/div/div[1]/div/div[1]/div/div[4]/div[2]/div"
        );
        apply?.Click();
    }

    public void AddFiltersToAccount()
    {
        var profileClass = "sidebar-profile";
        driver.FindElement(By.ClassName(profileClass))?.Click();

        var aboutMeXpath = "//*[@id='main']/div/div[1]/main/div/div[1]/div/section[4]";
        waitAndGet(aboutMeXpath)?.Click();

        var aboutMeContents = Account.AboutMe;

        var aboutMeInputXpath =
            "//*[@id='main']/div/div[1]/main/div/div[1]/div/section[4]/div/div/div/section/div/div/div/div/textarea[1]";
        var aboutMeInput = waitAndGet(aboutMeInputXpath);
        aboutMeInput?.Clear();
        aboutMeInput?.SendKeys(aboutMeContents);

        var saveXpath =
            "//*[@id='main']/div/div[1]/main/div/div[1]/div/section[4]/div/div/div/section/div/div[2]/div[2]/div[2]/div";
        waitAndGet(saveXpath)?.Click();

        var myPromptsXpath = "//*[@id='main']/div/div[1]/main/div/div[1]/div/section[3]";
        waitAndGet(myPromptsXpath)?.Click();

        // prompt 1
        var firstPromptSelectior =
            "#main > div > div.page__layout > main > div > div.scroll__inner > div > section.settings-section.is-opened > div > div > div > section > div > div > div > div.settings-cell__heading > span";

        Thread.Sleep(TimeSpan.FromSeconds(randomRange(1, 2)));

        waitAndGet("//div[@data-qa-role='settings-cell']")?.Click();

        Thread.Sleep(TimeSpan.FromSeconds(randomRange(0.5f, 1)));

        var divs = driver.FindElements(By.ClassName("options"));

        var lst = divs.Last()
            .FindElements(By.XPath("//div[@data-qa-role='option']"))
            .AsEnumerable();

        lst = lst.Where(e => e is not null && !string.IsNullOrEmpty(e.Text));

        Thread.Sleep(TimeSpan.FromSeconds(randomRange(2, 3)));

        var firstText = Account.Prompt1.Header;
        foreach (var e in lst)
        {
            if (e.Text.Contains(firstText))
            {
                e.Click();
                break;
            }
        }

        var tmpContents = Account.Prompt1.Description;
        var inputXpath =
            "//*[@id='main']/div/div[1]/main/div/div[1]/div/section[3]/div/div/div/section/div/div/section/div/div[1]/div/div/textarea[1]";

        try
        {
            waitAndGet(inputXpath)?.SendKeys(tmpContents);
        }
        catch (Exception ex) { }

        waitAndGet("//span[contains(text(), 'Save')]")?.Click();

        Thread.Sleep(TimeSpan.FromSeconds(randomRange(2, 3)));

        // prompt 2
        var secondProfilePrompt =
            "//*[@id='main']/div/div[1]/main/div/div[1]/div/section[3]/div/div/div/section/div/div[3]/div/div[1]/span";

        waitAndGet(secondProfilePrompt)?.Click();

        divs = driver.FindElements(By.ClassName("options"));

        lst = divs.Last().FindElements(By.XPath("//div[@data-qa-role='option']")).AsEnumerable();

        lst = lst.Where(e => e is not null && !string.IsNullOrEmpty(e.Text));

        Thread.Sleep(TimeSpan.FromSeconds(randomRange(2, 3)));
        var secondText = Account.Prompt2.Header;
        foreach (var e in lst)
        {
            if (e.Text.Contains(secondText))
            {
                e.Click();
                break;
            }
        }

        try
        {
            driver.FindElement(By.CssSelector(Account.Prompt2.Header))?.Click();
        }
        catch (Exception e) { }

        tmpContents = Account.Prompt2.Description;
        inputXpath =
            "//*[@id='main']/div/div[1]/main/div/div[1]/div/section[3]/div/div/div/section/div/div[3]/section/div/div[1]/div/div/textarea[1]";

        try
        {
            waitAndGet(inputXpath)?.SendKeys(tmpContents);
        }
        catch (Exception ex) { }
        waitAndGet("//span[contains(text(), 'Save')]")?.Click();

        Thread.Sleep(TimeSpan.FromSeconds(randomRange(2, 3)));

        // third profile prompt
        var thirdProfilePrompt =
            "//*[@id='main']/div/div[1]/main/div/div[1]/div/section[3]/div/div/div/section/div/div[4]/div/div[1]/span";

        waitAndGet(thirdProfilePrompt)?.Click();

        divs = driver.FindElements(By.ClassName("options"));

        lst = divs.Last().FindElements(By.XPath("//div[@data-qa-role='option']")).AsEnumerable();

        lst = lst.Where(e => e is not null && !string.IsNullOrEmpty(e.Text));

        Thread.Sleep(TimeSpan.FromSeconds(randomRange(2, 3)));

        var thirdText = Account.Prompt3.Header;
        foreach (var e in lst)
        {
            if (e.Text.Contains(thirdText))
            {
                e.Click();
                break;
            }
        }

        tmpContents = Account.Prompt3.Description;
        inputXpath =
            "//*[@id='main']/div/div[1]/main/div/div[1]/div/section[3]/div/div/div/section/div/div[4]/section/div/div[1]/div/div/textarea[1]";

        try
        {
            waitAndGet(inputXpath)?.SendKeys(tmpContents);
        }
        catch (Exception ex) { }
        waitAndGet("//span[contains(text(), 'Save')]")?.Click();

        Thread.Sleep(TimeSpan.FromSeconds(randomRange(2, 3)));

        var workEducationXpath = "/html/body/div/div/div[1]/main/div/div[1]/div/section[5]";
        waitAndGet(workEducationXpath)?.Click();
        var addAJobXpath =
            "/html/body/div/div/div[1]/main/div/div[1]/div/section[5]/div/div/div[1]/section/div/div/div";
        waitAndGet(addAJobXpath)?.Click();

        waitAndGet(
            "/html/body/div/div/div[1]/div[1]/div/div[2]/div/div/div[2]/form/div[1]/div/div/input"
        )
            ?.SendKeys(Account.JobTitle);

        Thread.Sleep(TimeSpan.FromSeconds(randomRange(1, 2)));

        waitAndGet(
            "/html/body/div/div/div[1]/div[1]/div/div[2]/div/div/div[2]/form/div[2]/div/div/input"
        )
            ?.SendKeys(Account.Company);

        waitAndGet("//span[contains(text(), 'Save')]")?.Click();

        Thread.Sleep(TimeSpan.FromSeconds(randomRange(0.5f, 1)));

        // education
        waitAndGet(
            "/html/body/div/div/div[1]/main/div/div[1]/div/section[5]/div/div/div[2]/section/div/div/div"
        )
            ?.Click();

        waitAndGet("//input[contains(@placeholder, 'Institution')]")?.SendKeys(Account.Institution);

        // grad year dropdown click
        var dropdown = waitAndGet(
            "/html/body/div/div/div[1]/div[1]/div/div[2]/div/div/div[2]/form/div[2]/div/div"
        );

        dropdown?.Click();

        var years = dropdown.FindElements(By.ClassName("option"));
        foreach (var year in years)
        {
            if (year is not null && year.Text.Contains("2022"))
            {
                year.Click();
                break;
            }
        }

        waitAndGet("//span[contains(text(), 'Save')]")?.Click();

        Thread.Sleep(TimeSpan.FromSeconds(randomRange(0.5f, 1)));

        // basic info
        waitAndGet("//*[@id='main']/div/div[1]/main/div/div[1]/div/section[6]")?.Click();

        // basic info menu path
        waitAndGet(
            "//*[@id='main']/div/div[1]/main/div/div[1]/div/section[6]/div/div/div/section[1]/div/div[1]/div"
        )
            ?.Click();

        // lets do it
        waitAndGet("//*[@id='main']/div/div[1]/div[1]/div/div[2]/div/div/section/div/div[3]/div")
            ?.Click();

        var slider = waitAndGet(
            "/html/body/div/div/div[1]/div[1]/div/div[2]/div/div/div/div/div/div/div[1]/div/div[2]/div/div[2]"
        );
        var actions = new Actions(driver);

        for (int i = 0; i < 3; i++)
        {
            actions.ClickAndHold(slider).MoveByOffset(100, 0).Release().Perform();
        }

        // yup thats how tall I am
        waitAndGet("/html/body/div/div/div[1]/div[1]/div/div[2]/div/div/div/div/div/div/div[2]/div")
            ?.Click();

        Thread.Sleep(1);

        // do you work out?
        waitAndGet("//label[@data-qa-role='choice-field--lifestyle_exercise--Active']")?.Click();

        // what's your education
        waitAndGet("//label[@data-qa-role='choice-field--lifestyle_education--Graduate']")
            ?.Click();

        // do you drink?
        waitAndGet("//label[@data-qa-role='choice-field--lifestyle_drinking--Never']")?.Click();

        // do you smoke?
        waitAndGet("//label[@data-qa-role='choice-field--lifestyle_smoking--Never']")?.Click();

        // What do you want from your dates?
        waitAndGet(
            "//label[@data-qa-role='choice-field--lifestyle_dating_intentions--Relationship']"
        )
            ?.Click();

        // What are your ideal plans for children?
        waitAndGet("//label[@data-qa-role='choice-field--lifestyle_family_plans--OpenToKids']")
            ?.Click();

        // What's your zodiac sign?
        waitAndGet("//label[@data-qa-role='choice-field--lifestyle_zodiak--Virgo']")?.Click();

        // What are your political leanings?
        waitAndGet("//label[@data-qa-role='choice-field--lifestyle_politics--Moderate']")?.Click();

        // Do you identify with a religion?
        waitAndGet("//label[@data-qa-role='choice-field--lifestyle_religion--Agnostic']")?.Click();
    }

    public void Dispose()
    {
        driver.Quit();
    }
}
