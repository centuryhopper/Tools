use std::fs::{self, metadata, read_dir};
use std::io::{self, BufRead, BufReader, ErrorKind, Write, Error, Result};
use std::path::{Path};
use chrono::{Datelike, Utc};
use chrono::prelude::*;
use chrono_tz::US::Eastern;
use std::process::Command;


// have an argument for journal (store private journal in journal github folder)



fn main() -> io::Result<()>{

    let JOURNAL_DIR = "/home/leo_zhang/Documents/GitHub/Journal".to_string();
    let root_path = Path::new(&JOURNAL_DIR);

    if !root_path.try_exists()?
    {
        let msg = format!("The root_path: {:?} doesn't exist", root_path.as_os_str());
        return Err(Error::new(ErrorKind::NotFound, msg));
    }

    // Get the current date and time in UTC
    let now_utc = Utc::now();

    // Convert the UTC time to Eastern Time
    let now_est = now_utc.with_timezone(&Eastern).format("%Y-%m-%d");

    println!("{:?}", &now_est.to_string());

    // Extract the year, month, and day
    let now_est_string = now_est.to_string();
    let year = now_est_string.split("-").nth(0);
    let month = now_est_string.split("-").nth(1);
    let day = now_est_string.split("-").nth(2);
    // println!("{}, {}, {}", year.unwrap(), month.unwrap(), day.unwrap());

    let year_month_dir = root_path.join(&year.unwrap()).join(&month.unwrap());

    if !year_month_dir.try_exists()? {
        fs::create_dir_all(&year_month_dir)?;
        println!("Directories created successfully: {:?}", year_month_dir.as_os_str());
    }

    let final_path = year_month_dir.join(format!("{}.md", &day.unwrap()));

     // Check if the file already exists
     if !final_path.try_exists()? {
        // Create the file if it doesn't exist
        fs::write(&final_path, "# New Journal Entry\n")?;
        println!("File created successfully: {:?}", final_path.as_os_str());
    } else {
        println!("File already exists: {:?}", final_path.as_os_str());
    }

    Command::new("nvim")
        .arg(final_path.to_str().unwrap())
        .status()?;

    Ok(())
}
