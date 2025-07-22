use chrono::prelude::*;
use chrono::{Datelike, Utc};
use chrono_tz::US::Eastern;
use std::fs::{self, metadata, read_dir, DirEntry};
use std::io::{self, BufRead, BufReader, Error, ErrorKind, Result, Write};
use std::path::Path;
use std::process::Command;

use std::sync::mpsc;
use std::sync::mpsc::{Receiver, Sender};
use std::thread::{self, JoinHandle};

// write a multi-threaded grep tool
// make recursive flag optional
// make sure no two threads are visiting the same directory/sub-directory

fn grep(dir: &str, target_file_name: &str, is_recursive: bool) -> io::Result<Vec<String>> {
    let root_path = Path::new(dir);
    if !root_path.try_exists()? {
        let msg = format!("The root_path: {:?} doesn't exist", root_path.as_os_str());
        // panic!("{}", msg);
        return Ok(vec![]);
    }

    let mut result = vec![];
    let mut entries: Vec<DirEntry> = fs::read_dir(root_path)?.filter_map(Result::ok).collect();

    entries.sort_by(|a, b| {
        a.file_name()
            .to_string_lossy()
            .to_lowercase()
            .cmp(&b.file_name().to_string_lossy().to_lowercase())
    });

    for entry in entries {
        let entry_full_path = entry.path();
        // println!("{:?}", entry_full_path);
        // drill into subdirectory
        if is_recursive && entry_full_path.is_dir() {
            result.extend(
                grep(entry_full_path.to_str().unwrap(), target_file_name, true)
                    .unwrap_or_else(|_| vec![]),
            );
        }

        if let Some(file_name) = entry_full_path.file_name() {
            // println!("{:?}",file_name);
            if file_name
                .to_string_lossy()
                .to_lowercase()
                .contains(&target_file_name.to_lowercase())
            {
                result.push(entry_full_path.to_string_lossy().into_owned());
            }
        }
    }

    return Ok(result);
}

fn main() {
    // println!("{:?}",home::home_dir().unwrap())

    let (tx, rx): (Sender<Vec<String>>, Receiver<Vec<String>>) = mpsc::channel();

    // just use up to 5 threads
    let dirs_to_search = std::env::args().skip(1).take(5).collect::<Vec<String>>();

    let mut thread_handles = vec![];

    for dir in dirs_to_search {
        let tx_clone = tx.clone();
        // println!("{}", dir.as_str());
        let t_handle = thread::spawn(move || {
            let results = grep(dir.as_str(), "test3", true).unwrap_or_else(|_| vec![]);
            tx_clone.send(results).ok();
        });
        thread_handles.push(t_handle);
    }

    // ✅ Drop the original sender here otherwise receiver hangs forever
    drop(tx);

    let mut results = vec![];

    for received in rx {
        results.extend(received);
    }

    for t in thread_handles {
        t.join().unwrap();
    }

    println!("results: {:#?}", results);

    // single threaded sample
    // let results = grep(
    //     home::home_dir()
    //         .unwrap()
    //         .join("Documents/GitHub")
    //         .to_str()
    //         .unwrap(),
    //     "test",
    //     true,
    // )
    // .unwrap_or_else(|_| vec![]);
    // println!("results: {:#?}", results);
}

// have an argument for journal (store private journal in journal github folder)
fn create_journal_entry() -> io::Result<()> {
    let JOURNAL_DIR = "/home/leo_zhang/Documents/GitHub/Journal".to_string();
    let root_path = Path::new(&JOURNAL_DIR);

    if !root_path.try_exists()? {
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
        println!(
            "Directories created successfully: {:?}",
            year_month_dir.as_os_str()
        );
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
