use time::OffsetDateTime;

use crate::job_traits::job_scraper_trait::job_scraper_trait;

pub struct PeopleFirstScraper;

impl job_scraper_trait for PeopleFirstScraper {
    fn start_url(&self) -> &str {
        "https://jobs.myflorida.com/go/Science%2C-Technology%2C-Engineering-&-Mathematics/2814200/"
    }

    fn should_visit(&self, url: &str) -> bool {
        if !url.contains("/job/") && !url.contains("/go/") {
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
        !url.contains("#content") && !url.contains("#") && !url.contains("/go/")
    }
    
    fn name(&self) -> String {
        let now = OffsetDateTime::now_utc();
        format!(
            "people_first_{:04}_{:02}_{:02}_{:02}_{:02}_{:02}.csv",
            now.year(),
            now.month() as u8,
            now.day(),
            now.hour(),
            now.minute(),
            now.second()
        )
    }
}
