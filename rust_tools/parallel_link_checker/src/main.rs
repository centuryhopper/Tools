use std::cmp::min;
use std::collections::HashSet;
use std::sync::{Arc, Mutex};
use std::thread;
use reqwest::blocking::{get, Response};
use reqwest::Url;
use scraper::{Html, Selector};
use thiserror::Error;
use std::sync::mpsc::{channel, Sender};


#[derive(Error, Debug)]
enum Error {
    #[error("request error: {0}")]
    ReqwestError(#[from] reqwest::Error),
}

/// yields a list of links parsed from the response object or an error if response link is invalid
fn extract_links(response: Response) -> Result<Vec<Url>, Error>
{
    let base_url = response.url().to_owned();
    let document = response.text()?;
    let html = Html::parse_document(&document);
    let selector = Selector::parse("a").unwrap();

    let mut valid_urls = Vec::new();
    for element in html.select(&selector) {
        if let Some(href) = element.value().attr("href") {
            match base_url.join(href) {
                Ok(url) => valid_urls.push(url),
                Err(err) => {
                    println!("On {base_url}: could not parse {href:?}: {err} (ignored)",);
                }
            }
        }
    }

    Ok(valid_urls)
}


/// focus on a designated slice of urls directed for this thread.
fn thread_work(slice: &[String], shared_state: Arc<Mutex<HashSet<String>>>, id: usize) {

    println!("thread {id} will process {} urls", slice.len());
    for url in slice {
        let start_url = Url::parse(url).unwrap();
        let response = get(start_url).unwrap();

        match extract_links(response) {
            Ok(links) => {

                for link in links
                {
                    let mut ds = shared_state.lock().unwrap();

                    let link_str = link.host_str().unwrap_or("bad_url");
                    if ds.contains(link_str) {
                        continue;
                    }
                    if ds.len() >= MAX_LINKS {
                        break;
                    }
                    println!("{}", ds.len());
                    ds.insert(link_str.to_owned());
                }
            }
            Err(err) => println!("Could not extract links: {err:#}"),
        }
    }
}


const MAX_LINKS : usize = 100;

#[allow(dead_code)]
#[rustfmt::skip]
fn main()
{
    let start_url = Url::parse("https://webscraper.io/test-sites/e-commerce/allinone").unwrap();
    let response = get(start_url).unwrap();
    let save_start = response.url().clone();
    let visited_urls = Arc::new(Mutex::new(HashSet::<String>::new()));
    let mut thread_handles = vec![];
    let mut initial_links = vec![];

    // initial extraction
    match extract_links(response) {
        Ok(links) => {

            // remove dupes
            let new_links = links.iter().map(|url| {
                format!("https://{}{}", url.host_str().unwrap_or("bad_url"), url.path())
            }).filter(|url| url != save_start.as_str()).collect::<HashSet<_>>();

            let mut visited = visited_urls.lock().unwrap();
            for link in &new_links {
                visited.insert(link.to_owned());
            }

            // println!("Links: {new_links:#?} \n number of links: {:?}", new_links.len());
            initial_links = new_links.into_iter().map(|x|x).collect::<Vec<_>>();
        }
        Err(err) => println!("Could not extract links: {err:#}"),
    }

    // println!("{:#?}", visited_urls.lock().unwrap());
    // println!("{:#?}", visited_urls.lock().unwrap().len());
    // println!("{:#?}", initial_links.len());
    // return;

    let mid = initial_links.len() / 2;

    for i in (0..initial_links.len()).step_by(if initial_links.len() % 2 == 1 {mid+1} else {mid})
    {
        let links_clone = initial_links.clone();
        let visited_clone = Arc::clone(&visited_urls);

        // i to (i + mid) slice
        println!("thread {i}");

        let thread_handle = thread::spawn(move || {
            thread_work(&links_clone[i..min(links_clone.len(), i+mid)], visited_clone, i);
        });

        thread_handles.push(thread_handle);
    }
        
    println!("herer");
    for handle in thread_handles {
        match handle.join() {
            Ok(_) => {
                println!("success thread join");
            }

            Err(_) => {
                println!("failed thread join");
            }
        }
    }

    println!("{:#?}", visited_urls.lock().unwrap());

}




/*
I would like some help with the correctness (or incorrectness) and improving upon my code for this problem: https://google.github.io/comprehensive-rust/exercises/concurrency/link-checker.html
There is no answer key for this one unlike the other exercises I have done so far. I am using locks and I don't know if my program is too slow or is this normal because of all the links it parses.
I am not doing any recursion yet. I just parse out all the available links that are present from a given url then add them to a vector that I called "initial_links" and then divide that up into slices. The same initially parsed links are also added into a hashset that I called "visited_urls" so that any newly found links that are already in the set get ignored. Each slice of urls from the vector will be handled by a thread and the urls that generate more urls would be added into visited_urls. So right now I only go one level below relative to the already parsed links.
Eventually I would like to continue with recursively checking the links that the already parsed links generate etc. but I haven't used recursion in multithreading before and I would like some pointers with that. Would it get messy? Bad practice?
Here's roughly what my program does (or should do):
-parse link (e.g. https://webscraper.io/test-sites/e-commerce/allinone)
-return a list of urls present on that link page: [a_url, b_url, c_url, ...]
-i divide that list in half and spawn 2 threads to take care of creating parsing urls from each one in their own slices
-once they have done so, they skip the already seen urls and add the newly found urls to "visited_urls", which I lock when adding as it is of type Arc<Mutex<HashSet<String>>>
-the thread will stop either when there are no more links to parse or when we have visited a grand total of 100 urls (for simplicity and sanity's sake)



 */