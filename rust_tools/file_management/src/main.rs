#![allow(warnings)]

// mod file_deduper_hashing;
// mod file_deduper_naive;
mod file_deduper_hashing_parallel;
mod grep;
mod journal;

// use file_deduper_hashing::find_duplicates_by_hashing;
// use file_deduper_naive::file_deduper_naive;
use file_deduper_hashing_parallel::{get_all_files, get_file_hashes, handle_duplicates};
use grep::grep;
use journal::create_journal_entry;

use chrono::prelude::*;
use chrono::{Datelike, Utc};
use chrono_tz::US::Eastern;
use std::collections::HashSet;
use std::ffi::OsString;
use std::fs::{self, metadata, read_dir, DirEntry, File};
use std::io::{self, BufRead, BufReader, BufWriter, Error, ErrorKind, Result, Write};
use std::path::{Path, PathBuf};
use std::process::Command;

use std::sync::mpsc;
use std::sync::mpsc::{Receiver, Sender};
use std::thread::{self, JoinHandle};
use std::time::Instant;
mod utils;
use utils::now;

use clap::{Parser, Subcommand};

#[derive(Parser, Debug)]
#[command(author, version, about = "Multi-threaded grep + journal CLI")]
pub struct Cli {
    #[command(subcommand)]
    pub command: Commands,
}

#[derive(Subcommand, Debug)]
pub enum Commands {
    /// Finds and groups duplicate files into a collection of file paths, where each collection corresponds to a unique file content hash. This is done by first scanning the specified directory and its subdirectories to gather all file paths, then computing the hash of each file's content in parallel using multiple threads, and finally grouping the file paths by their computed hashes to identify duplicates.
    #[command(name = "FileDeduper")]
    FileDeduper {
        /// Directory to scan for duplicate files
        #[arg(required = true, short, long, default_value = "./test_duplicates")]
        path: String,

        #[arg(short, long)]
        delete: bool,

        #[arg(short = 'e', long = "exclude", value_delimiter = ',')]
        exclude: Vec<String>,
    },

    /// Search directories for file names
    Grep {
        /// Directories to search
        #[arg(required = true)]
        dirs: Vec<String>,

        /// Target file name
        #[arg(short = 't', long, default_value = "test3")]
        target: String,

        /// Recursive search
        #[arg(short, long)]
        recursive: bool,

        /// Max threads
        #[arg(short = 'j', long = "threads", default_value_t = 5)]
        threads: usize,
    },

    /// Create today's journal entry
    Journal,
}

fn run_grep(dirs_to_search: Vec<String>, target: String, recursive: bool, threads: usize) {
    // println!("{:?}",home::home_dir().unwrap())

    let (tx, rx): (Sender<Vec<String>>, Receiver<Vec<String>>) = mpsc::channel();

    // just use up to 5 threads
    // make sure no two threads are visiting the same directory/sub-directory
    // let dirs_to_search = std::env::args()
    //     .skip(1)
    //     .take(5)
    //     .collect::<HashSet<String>>();

    println!("dirs_to_search: {:#?}", dirs_to_search);

    let mut thread_handles = vec![];

    for dir in dirs_to_search {
        let tx_clone = tx.clone();
        // println!("{}", dir.as_str());
        // multi-threaded grep tool
        let target_str = target.clone();
        let t_handle = thread::spawn(move || {
            let results = grep(dir.as_str(), target_str.as_str(), recursive).unwrap_or_default();
            // .unwrap_or_else(|_| vec![]);
            tx_clone.send(results).ok();
        });
        thread_handles.push(t_handle);
    }

    // Drop the original sender here otherwise receiver hangs forever
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

fn run_cli() -> std::io::Result<()> {
    let cli = Cli::parse();

    match cli.command {
        // example usage:
        /*
            cargo run -- grep ./test -r -t test3 -j 2
            cargo run -- grep ./test --recursive --target test3 --threads 2
        */
        Commands::Grep {
            dirs,
            target,
            recursive,
            threads,
        } => run_grep(dirs, target, recursive, threads),

        Commands::Journal => {
            journal::create_journal_entry().unwrap();
        }

        // cargo run -- FileDeduper -dp ./test_duplicates -e "test3.txt,subdir/test3.txt"
        // cargo run -- FileDeduper -dp ~/synology/ -e '#recycle'
        Commands::FileDeduper {
            path,
            delete,
            exclude,
        } => {
            println!("delete flag: {}", delete);
            println!("[{}] Collecting all files...", now());
            let results = get_all_files(path.as_str(), &exclude).unwrap_or_default();
            println!("[{}] Number of files collected: {}", now(), results.len());
            let file_hashes = get_file_hashes(&results, &exclude);
            
            println!("[{}] Handling duplicates...", now());
            handle_duplicates(&file_hashes, delete).unwrap_or_else(|e| {
                eprintln!("Error during handling duplicates: {}", e);
            });
            println!("[{}] Finished handling duplicates.", now());
        }
    }

    Ok(())
}

fn main() -> io::Result<()> {
    
    // let mut test = BufWriter::new(File::create("./output/test.txt")?);
    // writeln!(test, "Test output")?;
    // test.flush()?;

    // return Ok(());

    // limit to just 4 threads for now, otherwise the program will be slow for large directories over a network drive since the program will spawn too many threads and overwhelm the network drive with too many requests at once.
    rayon::ThreadPoolBuilder::new()
        .num_threads(4)
        .build_global()
        .unwrap();

    let start = Instant::now();

    run_cli()?;

    let duration = start.elapsed();
    println!("Time elapsed: {:?}", duration);

    Ok(())
}

/*

Main Thread (Scanner)
┌─────────────────────────────┐
│Walk directories recursively │
└─────────────┬───────────────┘
              │ file path
              ▼
          Queue / Channel
   ┌─────────┴─────────┐
   │                   │
Worker Thread 1     Worker Thread 2   ... Worker Thread N
   │                   │
   ▼                   ▼
Compute hash         Compute hash
   │                   │
   └───────► Results Collector ◄─────┘
               (group by hash)




*/
