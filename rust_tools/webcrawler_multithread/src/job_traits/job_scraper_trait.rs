use time::OffsetDateTime;



pub trait job_scraper_trait {
    // This method should return the starting URL for the scraper.
    fn start_url(&self) -> &str;

    // return the name of the csv file exported
    fn name(&self) -> String {
        let now = OffsetDateTime::now_utc();
        format!(
            "results_{:04}_{:02}_{:02}_{:02}_{:02}_{:02}.csv",
            now.year(),
            now.month() as u8,
            now.day(),
            now.hour(),
            now.minute(),
            now.second()
        )
    }
    
    fn should_visit(&self, url: &str) -> bool {
        true
    }

    fn results_filter(&self, url: &str) -> bool {
        true
    }
}