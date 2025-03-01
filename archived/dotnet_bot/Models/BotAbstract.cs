
using OpenQA.Selenium;

namespace dotnet_bot.Abstract;

public abstract class BotAbstract
{
    public abstract IWebElement? WaitAndGet(string xpath, float seconds=2.5f);
    public abstract IWebElement[]? WaitAndGetAll(string xpath, float seconds=5f);
    public abstract void Screenshot();
    public abstract bool TryLogin();
    public abstract void Dispose();
    public abstract void HandlePopups(int milliseconds=0);
    public abstract void SendMessageToMatches();
    public abstract bool Like();
    public abstract bool DisLike();
    public abstract bool NoMoreLikes();
    public abstract bool RanOutOfMatches();
    public abstract void AutoSwipe();

    public virtual IWebElement? GetButtonParent(IWebElement? e)
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
}

