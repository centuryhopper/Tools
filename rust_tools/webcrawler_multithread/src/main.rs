use std::{collections::HashSet, sync::mpsc};

use futures::stream::{FuturesUnordered, StreamExt};
use reqwest::Client;
use scraper::{Html, Selector};
use std::collections::VecDeque;
use tokio::task;
use url::Url;

/*
implement a multi-threaded web crawler to crawl all links that are under the same hostname as "start_url"

return all urls obtained by your web crawler in any order

steps:
    - start the page from start_url
    - call HtmlParser.getUrls(url) to get all urls from a webpage of a given url
    - do not crawl the same link twice
    - explore only the links that are under the same hostname as starturl
*/

struct HtmlParser;

impl HtmlParser {
    pub async fn fetch_links(client: &Client, url: &str) -> Result<Vec<String>, reqwest::Error> {
        // println!("url: {url}");
        let body = client.get(url).send().await?.text().await?;
        let document = Html::parse_document(&body);
        let selector = Selector::parse("a").unwrap();

        let base = Url::parse(url).expect("couldn't parse url"); // parse the base URL

        let links = document
            .select(&selector)
            .filter_map(|el| el.value().attr("href"))
            .filter_map(|href| base.join(href).ok()) // convert to absolute
            .map(String::from)
            .collect();

        // if url == "https://books.toscrape.com/catalogue/page-2.html" {
        //     println!("base: {:?}", base);
        //     println!("links: {:?}", links);
        // } else {
        //     println!("url: {:?}", url);
        //     println!("links: {:?}", links);
        // }

        // println!("links: {:?}", links);

        Ok(links)
    }
}

// BFS approach

/*
// TODO: For an async web crawler, use channels to send discovered URLs back to a managing task that owns the seen set and the deque. It’s clean, idiomatic, and easier to maintain.
*/
async fn crawl(start_url: &str) -> Result<(), Box<dyn std::error::Error>> {
    let base_host = Url::parse(start_url)?
        .host_str()
        .ok_or("Invalid start URL hostname")?
        .to_string();
    // println!("base_host: {base_host}");
    let mut seen: HashSet<String> = HashSet::from([start_url.to_string()]);
    let mut deque: VecDeque<String> = VecDeque::from([start_url.to_string()]);

    let client = Client::new();
    // a futures pool
    let mut tasks = FuturesUnordered::new();

    while seen.len() < 2000 && (!deque.is_empty() || !tasks.is_empty()) {
        // println!("deque size: {}", deque.len());
        // println!("tasks size: {}", tasks.len());
        while let Some(url) = deque.pop_front() {
            let client = client.clone();
            tasks.push(task::spawn(async move {
                let links = HtmlParser::fetch_links(&client, url.as_str()).await;
                (url, links)
            }));
        }
        if let Some(res) = tasks.next().await {
            match res {
                Ok((_, Ok(links))) => {
                    // println!("{} -> {} links", url, links.len());
                    // println!("links: {:?}", links);
                    for link in links {
                        let link_base = Url::parse(&link)?
                            .host_str()
                            .ok_or("Invalid URL")?
                            .to_string();
                        // println!("link: {link}");
                        if link_base == base_host && seen.insert(link.clone()) {
                            deque.push_back(link);
                        }
                    }
                }
                Ok((url, Err(e))) => {
                    eprintln!("Error on {}: {}", url, e);
                }
                Err(e) => eprintln!("Task panicked: {}", e),
            }
        }
    }

    // println!("{:?}", seen.len());
    // println!("{:?}", seen);

    Ok(())
}

#[tokio::main(flavor = "multi_thread")]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    crawl("https://books.toscrape.com/").await
}
