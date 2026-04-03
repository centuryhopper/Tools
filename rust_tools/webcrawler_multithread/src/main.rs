#[allow(warnings)]

use std::{collections::HashSet, fs, path::Path, sync::{Arc, mpsc}};

use futures::stream::{FuturesUnordered, StreamExt};
use reqwest::{Client, header::{HeaderMap, HeaderValue, USER_AGENT}};
use scraper::{Html, Selector};
use time::OffsetDateTime;
use std::collections::VecDeque;
use tokio::{sync::Semaphore, task};
use url::Url;

use std::sync::atomic::{AtomicUsize, Ordering};
use csv::{Writer, WriterBuilder};
use std::fs::File;

mod job_traits;
mod scrapers;

use job_traits::job_scraper_trait::job_scraper_trait;
use scrapers::publix_job_scraper::PublixJobScraper;
use scrapers::people_first_scraper::PeopleFirstScraper;
use scrapers::ucf_job_scraper::UCFJobScraper;


/*

TODO add these urls to the list of urls to crawl:
    https://jobs.universalparks.com/job-search-results/?addtnl_categories[]=Digital%20Technology&location=FL%2C%20Orlando

implement a multi-threaded web crawler to crawl all links that are under the same hostname as "start_url"

return all urls obtained by your web crawler in any order

steps:
    - start the page from start_url
    - call HtmlParser.getUrls(url) to get all urls from a webpage of a given url
    - do not crawl the same link twice
    - explore only the links that are under the same hostname as starturl


visualization:
    - start_url -> [link1, link2, ...]
    - link1 -> [link3, link4, ...]
    - link2 -> [link5, link6, ...]
    - ...

            [ main task ]
           /      |      \
      spawn   spawn   spawn
        |        |        |
     worker   worker   worker
        |        |        |
        -------- results ------>
                    |
            (main task updates state)
*/

struct HtmlParser;

impl HtmlParser {
    pub async fn fetch_links(client: &Client, url: &str) -> Result<Vec<String>, reqwest::Error> {
        let mut headers = HeaderMap::new();

        headers.insert(
            USER_AGENT,
            HeaderValue::from_static(
                "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/146.0.0.0 Safari/537.36 Edg/146.0.0.0"
            ),
        );

        headers.insert("Accept", HeaderValue::from_static("text/html"));
        headers.insert("Accept-Language", HeaderValue::from_static("en-US,en;q=0.9"));
        headers.insert("Connection", HeaderValue::from_static("keep-alive"));

        let body = client
            .get(url)
            .headers(headers)
            .send()
            .await?
            .text()
            .await?;
        let document = Html::parse_document(&body);
        println!("document: {:?}", document.html());
        let selector = Selector::parse("a").unwrap();

        let base = Url::parse(url).expect("couldn't parse url"); // parse the base URL

        let links = document
            .select(&selector)
            .filter_map(|el| el.value().attr("href"))
            .filter_map(|href| base.join(href).ok()) // convert to absolute
            .map(String::from)
            .collect();

        Ok(links)
    }
}

// BFS approach
async fn crawl(job_scraper: Box<dyn job_scraper_trait>) -> Result<(Vec<String>, String), Box<dyn std::error::Error>> {
    let base_host = Url::parse(job_scraper.start_url())?
        .host_str()
        .ok_or("Invalid start URL hostname")?
        .to_string();
    // println!("base_host: {base_host}");

    /*
    We dont need concurrent data structures like Mutex or RwLock here because we are using a single owner + worker pool model:
        Workers do stateless work
        Main task owns all mutable state
    */
    let mut seen: HashSet<String> = HashSet::from([job_scraper.start_url().to_string()]);
    let mut deque: VecDeque<String> = VecDeque::from([job_scraper.start_url().to_string()]);

    let client = Client::new();
    /*
        Why it’s necessary here

        Your crawler loops over potentially thousands of URLs. Without a semaphore:

        You might spawn hundreds or thousands of tasks at once.
        Each task makes an HTTP request → overload your network, CPU, or the target servers.
        You could get rate-limited or crash your program.

        With the semaphore:

        You limit concurrency to 20 fetches.
        The rest of the tasks wait in line.
        This keeps your program efficient and polite to the websites you crawl.


        Quick analogy

        Imagine a fast-food restaurant:

        Only 20 people can cook at the same time (20 permits).
        Others wait in line until someone finishes cooking.
        If you didn’t limit it, 200 people could all start cooking → chaos.

        Here, Semaphore = control the number of simultaneous network requests.
    
     */
    let semaphore = Arc::new(Semaphore::new(20)); // max 20 concurrent fetches

    // a futures pool
    let mut tasks = FuturesUnordered::new();

    // let skipped_count = Arc::new(AtomicUsize::new(0));

    // we'll just limit to 2000 for now
    while seen.len() < 2000 && (!deque.is_empty() || !tasks.is_empty()) {
        // println!("deque size: {}", deque.len());
        // println!("tasks size: {}", tasks.len());
        while let Some(url) = deque.pop_front() {
            let client = client.clone();
            let permit = semaphore.clone().acquire_owned().await.unwrap();
            tasks.push(task::spawn(async move {
                // This is a trick to keep the permit alive for the duration of the task.
                // Once the task ends (or _permit is dropped), the permit is automatically released, allowing another task to start.
                let _permit = permit; // hold permit while task runs
                let links = HtmlParser::fetch_links(&client, url.as_str()).await;
                println!("fetched links : {:?}", links);
                (url, links)
            }));
        }

        if let Some(res) = tasks.next().await {
            match res {
                Ok((_, Ok(links))) => {
                    println!("fetched {} links", links.len());
                    // println!("links: {:?}", links);

                    for link in links {

                        // skip if the link is not relevant to job postings
                        // println!("checking url: {}", link);
                        // println!("base_host: {}", base_host);
                        if link != base_host
                        {
                            if !job_scraper.should_visit(link.as_str())
                            {
                                continue;
                            }
                        }

                        if let Ok(u) = Url::parse(&link) {
                            if let Some(host) = u.host_str() {
                                // make sure the link is under the same hostname and we haven't seen it before
                                if host == base_host && seen.insert(link.clone()) {
                                    deque.push_back(link);
                                }
                            }
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

    // println!("number of skipped links: {}", skipped_count.load(Ordering::Relaxed));
    let results = seen.iter()
    .filter(|link| job_scraper.results_filter(link.as_str()))
    .cloned()
    .collect::<Vec<_>>();
    // println!("{:?}", results);
    // println!("{:?}", results.len());

    let filename = format!("out/{}",job_scraper.name());

    Ok((results, filename))
}

fn save_to_csv(filename: &str, results: &[String]) -> Result<(), Box<dyn std::error::Error>> {
    // --- Export results to CSV ---
    let out_dir = Path::new("out");
    if !out_dir.exists() {
        fs::create_dir_all(out_dir)?;
    }
    // CSV writer: comma-delimited and quotes all fields
    let mut wtr = WriterBuilder::new()
        .delimiter(b',')
        .quote_style(csv::QuoteStyle::Always) // <-- QUOTE EVERY FIELD
        .from_writer(File::create(&filename)?);

    // Write header
    wtr.write_record(&["URL"])?;

    // Write rows
    for url in results {
        wtr.write_record(&[url])?;
    }

    wtr.flush()?;
    println!("Results saved to {}", filename);

    Ok(())
}

#[tokio::main(flavor = "multi_thread")]
async fn main() -> Result<(), Box<dyn std::error::Error>> {

    let scrapers = vec![
        // Box::new(PeopleFirstScraper) as Box<dyn job_scraper_trait>,
        // Box::new(PublixJobScraper) as Box<dyn job_scraper_trait>,
        Box::new(UCFJobScraper) as Box<dyn job_scraper_trait>,
    ];

    let mut accummulated_results : Vec<String> = vec![];

    for scraper in scrapers {
        let (results, _) = crawl(scraper).await?;
        accummulated_results.extend(results);
    }
    let now = OffsetDateTime::now_utc();
    let filename = format!(
            "out/total_results_{:04}_{:02}_{:02}_{:02}_{:02}_{:02}.csv",
            now.year(),
            now.month() as u8,
            now.day(),
            now.hour(),
            now.minute(),
            now.second()
        );
    save_to_csv(filename.as_str(), &accummulated_results)?;
    Ok(())
}
