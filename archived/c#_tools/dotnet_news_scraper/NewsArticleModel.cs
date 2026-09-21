namespace dotnet_news_scraper.Models;

public class NewsArticleModel
{
    public string Title {get;set;}
    public string Author {get;set;}
    public string Desc {get;set;}
    public string Url {get;set;}
    public string PublishedAt {get;set;}

    public override string ToString()
    {
        return $"{nameof(Title)}:{Title}, {nameof(Author)}:{Author}, {nameof(Desc)}:{Desc}, {nameof(Url)}:{Url}, {nameof(PublishedAt)}:{PublishedAt}";
    }
}




