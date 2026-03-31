use time::OffsetDateTime;

use crate::job_traits::job_scraper_trait::job_scraper_trait;


pub struct PublixJobScraper;

impl job_scraper_trait for PublixJobScraper {
    fn start_url(&self) -> &str {
        "https://jobs.publix.com/jobs/?p=1&per_page=100&pg=1&department_type_slug=publix-technology"
    }

    fn should_visit(&self, url: &str) -> bool {
        if !url.contains("job-opening") {
            return false;
        }

        if url.contains("#main") {
            return false;
        }

        let keywords = [
            "developer",
            "engineer",
            "architect",
            "developer",
            "analyst",
            "analytics",
            "technician",
            "administrator",
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
        !url.contains("fromsearch") && !url.contains("#main")
    }
    
    fn name(&self) -> String {
        let now = OffsetDateTime::now_utc();
        format!(
            "publix_tech_{:04}_{:02}_{:02}_{:02}_{:02}_{:02}.csv",
            now.year(),
            now.month() as u8,
            now.day(),
            now.hour(),
            now.minute(),
            now.second()
        )
    }
}
