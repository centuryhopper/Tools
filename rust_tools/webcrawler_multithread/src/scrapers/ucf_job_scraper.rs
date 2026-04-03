use time::OffsetDateTime;

use crate::job_traits::job_scraper_trait::job_scraper_trait;

pub struct UCFJobScraper;

impl job_scraper_trait for UCFJobScraper {
    fn start_url(&self) -> &str {
        "https://jobs.ucf.edu/jobs/search"
    }

    fn should_visit(&self, url: &str) -> bool {

        if !url.contains("/job/") {
            return false;
        }

        let keywords = [
            "analyst",
            "programmer",
            "specialist",
            "data",
            "computer",
            "information",
            "tech",
        ];

        // URL must contain at least one of the keywords to be considered relevant
        if !keywords
            .iter()
            .any(|kw| url.to_lowercase().contains(&kw.to_lowercase()))
        {
            return false; // skip this URL
        }

        true
    }

    fn results_filter(&self, url: &str) -> bool {
        // println!("filtering url: {}", url);
        true
        // !url.contains("/jobs/search") && !url.contains("#") && !url.contains("/go/")
    }
}
