use std::fs::File;
use std::io::prelude::*;
use std::result::Result;
use reqwest::get;
use std::path::{Path, PathBuf};
use std::env;
use serde::{Serialize, Deserialize};
use thiserror::Error;
use chrono::prelude::*;

#[derive(Error, Debug)]
enum Error {
    #[error("no image for today. Error: {0}")]
    ReqwestError(#[from] reqwest::Error),
}

fn get_filename() -> PathBuf {
    let directory = env::current_dir().unwrap().join("nasa_daily_pic_collection");
    let filename = "FILENAME";

    directory.join(filename)
}

#[derive(Debug, Serialize, Deserialize)]
struct NasaPic
{
    #[serde(rename = "date")]
    Date: String,
    #[serde(rename = "explanation")]
    Explanation: String,
    #[serde(rename = "hdurl")]
    Hdurl: String,
    #[serde(rename = "media_type")]
    Media_type: String,
    #[serde(rename = "service_version")]
    Service_version: String,
    #[serde(rename = "title")]
    Title: String,
    #[serde(rename = "url")]
    Url: String,
}

impl NasaPic {
    fn to_string(&self) -> String {
        format!("Date: {}\n\nExplanation: {}\n\nHdurl: {}\n\nMedia_type: {}\n\nService_version: {}\n\nTitle: {}\n\nUrl: {}", self.Date, self.Explanation, self.Hdurl, self.Media_type, self.Service_version, self.Title, self.Url)
    }
}

async fn download_pic_of_the_day() -> Result<(), Error> {
    const PIC_COLLECTION_NAME : &str = "nasa_pic_collection";
    let pic_collection_path = Path::new(PIC_COLLECTION_NAME);
    let today_string = Local::now().format("%Y-%m-%d").to_string();
    let today_folder_name = pic_collection_path.join(&today_string);

    // create a folder called nasa_pic_collection/ if it doesn't exist already
    if !pic_collection_path.exists() {
        std::fs::create_dir(pic_collection_path).unwrap_or_else(|err| panic!("couldn't create {} directory. Error: {}", PIC_COLLECTION_NAME, err));
    }

    // create folder with today's date (if it doesn't exist already) and put into nasa_pic_collection/ directory
    if !today_folder_name.exists() {
        std::fs::create_dir(&today_folder_name).unwrap_or_else(|err| panic!("couldn't create {} directory. Error: {}", today_string, err));
    }

    let url = "https://api.nasa.gov/planetary/apod?api_key=DEMO_KEY";
    let response = get(url).await?;
    if !response.status().is_success() {
        // println!("error");
        //// return with an error saying there's no image today
        response.error_for_status_ref()?;
    }
    
    // used to figure out the json properties
    // let text = response.text().await?;
    // println!("{:?}", text);

    let json_output : NasaPic = response.json().await?;
    // println!("{:#?}", json_output);

    // grab each parsed property and store into text file
    // store that text file into folder named today's date

    // Open the file in write mode, creating it if it doesn't exist
    let filename = today_folder_name.join(format!("{}.txt", today_string));
    let mut file = File::create(&filename).expect("Failed to create file");

    // Write the content to the file
    file.write_all(json_output.to_string().as_bytes()).expect("Failed to write to file");

    // save picture to that folder as well
    let image_url = json_output.Url;
    let image_response = get(image_url).await?;

    if image_response.status().is_success() {
        // Open a file for writing
        let pic_filename = "nasa_pic_".to_owned() + today_string.as_str() + ".jpg";
        // println!("image file path: {}", today_folder_name.join(&pic_filename).to_string_lossy().to_string());
        let mut file = File::create(today_folder_name.join(&pic_filename))
            .expect("Failed to create file");

        // Copy the response body to the file
        let content = image_response.bytes().await?;

        // Write the image bytes to the file
        file.write_all(&content).unwrap_or_else(|err| panic!("failed to download image. Error: {}", err));
    }

    Ok(())
}


#[tokio::main]
async fn main() -> Result<(), Error> {

    // let parent_dir = Path::new(file!()).parent().unwrap_or_else(|| Path::new(""));
    // let parent_dir_str = parent_dir.to_string_lossy().to_string();
    // println!("Parent directory: {}", parent_dir_str);

    if let Some(parent_dir) = Path::new(file!()).parent() {
        env::set_current_dir(parent_dir).expect("Failed to change directory");

        download_pic_of_the_day().await?;

        return Ok(());
    }

    Ok(())
}
