use thirtyfour::prelude::*;
use tokio;
use std::time::Duration;
use tokio::time::sleep;

#[tokio::main]
async fn main() -> WebDriverResult<()>
{
    let mut caps = DesiredCapabilities::firefox();

    //let res = caps.set_headless();

    //match res {
    //    Ok(_) =>{
    //        println!("successfully set browser on headless");
    //    }

    //    Err(_) => {
    //        println!("failure to set browser on headless");
    //    }
    //}

    let driver = WebDriver::new("http://localhost:4444", caps).await?;

    // Navigate to https://wikipedia.org.
    driver.goto("https://wikipedia.org").await?;
    let elem_form = driver.find(By::Id("search-form")).await?;

    // Find element from element.
    let elem_text = elem_form.find(By::Id("searchInput")).await?;

    // Type in the search terms.
    elem_text.send_keys("selenium").await?;

    // Click the search button.
    let elem_button = elem_form.find(By::Css("button[type='submit']")).await?;
    elem_button.click().await?;

    // Look for header to implicitly wait for the page to load.
    driver.find(By::ClassName("firstHeading")).await?;
    assert_eq!(driver.title().await?, "Selenium - Wikipedia");

    println!("title: {}", driver.title().await?);
    println!("firstHeading: {}", (driver.find(By::ClassName("firstHeading")).await?).text().await?);


     // Delay for 2 seconds
    sleep(Duration::from_secs(5)).await;
    
    // Always explicitly close the browser. There are no async destructors.
    driver.quit().await?;

    Ok(())
}
