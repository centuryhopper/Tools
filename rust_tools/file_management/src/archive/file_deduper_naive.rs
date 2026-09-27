use std::collections::HashSet;
use std::ffi::OsString;
use std::fs::{self, DirEntry};
use std::path::Path;
use std::io::{self, BufRead, BufReader, Error, ErrorKind, Result, Write};

    // if let Err(e) = file_deduper() {
    //     eprintln!("Error during file deduplication: {}", e);
    // }
    // let duplicates = file_deduper().unwrap_or_else(|_| vec![]);
    // println!("Found duplicates: {:#?}", duplicates);
    // let duplicates = file_deduper_naive();

    // let entries: Vec<DirEntry> = fs::read_dir(Path::new("./test_duplicates"))?
    //     .filter_map(Result::ok)
    //     .collect();
    // let duplicates = find_duplicates_by_hashing(&entries);

    // match duplicates {
    //     Ok(dups) => {
    //         println!("Found duplicates: {:#?}", dups);
    //     }
    //     Err(e) => {
    //         eprintln!("Error during file deduplication: {}", e);
    //     }
    // };


    // println!("Number of unique hashes: {}", file_hashes.len());

            // let duplicates: Vec<_> = file_hashes.values().filter(|v| v.len() > 1).collect();
            // let output = format!("Duplicate groups: {:#?}", duplicates);

            // println!("duplicates written to text file.");

            // let mut buffer = BufWriter::new(File::create("duplicates.txt")?);

            // for group in &duplicates {
            //     for path in *group {
            //         writeln!(buffer, "{}", path.display())?;
            //     }
            //     writeln!(buffer)?; // blank line between groups
            // }

            // // println!("Found files: {:#?}", results);

            // buffer.flush()?;

fn files_identical_simple(path1: &str, path2: &str) -> std::io::Result<bool> {
    let b1 = fs::read(path1)?;
    let b2 = fs::read(path2)?;
    Ok(b1 == b2)
}



pub fn file_deduper_naive() -> io::Result<Vec<Vec<String>>> {
    // search for duplicate files in a directory based on file contents
    let search_dir = "./test_duplicates";
    let mut duplicates: Vec<Vec<String>> = vec![];
    // println!("here");
    // ignores entries that failed to read and keeps the good ones
    let entries: Vec<DirEntry> = fs::read_dir(Path::new(search_dir))?.filter_map(Result::ok).collect();

    println!("Scanning entries: {:<#?}", entries.iter().map(|e| e.file_name()).collect::<Vec<OsString>>());

    let mut seen : HashSet<String> = HashSet::new();

    for entry in &entries
    {
        let entry_full_path = entry.path();
        if entry_full_path.is_file() {
            let mut duplicate_group: Vec<String> = vec![];
            let entry_str = entry_full_path.to_string_lossy().into_owned();
            println!("Checking file: {}", entry_str);
            // don't process if already seen
            if seen.contains(&entry_str) {
                continue;
            }
            // mark as visited
            seen.insert(entry_str.clone());
            duplicate_group.push(entry_str.clone());

            for other_entry in &entries {
                let other_entry_full_path = other_entry.path();
                if other_entry_full_path.is_file() && other_entry_full_path != entry_full_path {
                    let other_entry_str = other_entry_full_path.to_string_lossy().into_owned();
                    if files_identical_simple(&entry_str, &other_entry_str)? {
                        // mark as seen
                        seen.insert(other_entry_str.clone());
                        duplicate_group.push(other_entry_str);
                    }
                }
            }

            if duplicate_group.len() > 1 {
                duplicates.push(duplicate_group);
            }
        }
    }

    
    Ok(duplicates)
}


