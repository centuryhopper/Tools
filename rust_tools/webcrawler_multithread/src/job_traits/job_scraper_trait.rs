

pub trait job_scraper_trait {
    // This method should return the starting URL for the scraper.
    fn start_url(&self) -> &str;

    fn name(&self) -> String; // <-- new method to return the name of the csv file exported
    
    fn should_visit(&self, url: &str) -> bool {
        true
    }

    fn results_filter(&self, url: &str) -> bool {
        true
    }
}