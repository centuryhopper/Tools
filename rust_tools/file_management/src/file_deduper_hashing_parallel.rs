use chrono::format;
use sha2::{Digest, Sha256};
use walkdir::WalkDir;
use std::collections::HashMap;
use std::fs::File;
use std::io::{Read, Result};
use std::path::PathBuf;
use rayon::prelude::*;

// sequentially find all the files by their absolute paths in the current directory and its subdirectories, compute their hashes in parallel, and group them by hash to find duplicates. 


pub fn get_all_files(str_path: &str) -> Result<Vec<PathBuf>> {
    let mut files = Vec::new();
    for entry in WalkDir::new(str_path) {
        let entry = entry?;
        if entry.file_type().is_file() {
            files.push(entry.path().to_path_buf());
        }
    }
    Ok(files)
}


pub fn get_file_hashes(files: &[PathBuf], exclude: &[String]) -> HashMap<Vec<u8>, Vec<PathBuf>> {

    /*
        Map → compute (hash, path)
        Fold → build local HashMap
        Reduce → merge the HashMaps
    */
    files
        .par_iter()
        // remove all files that cannot be hashed (e.g. permission issues)
        .filter_map(|path| {

            // check if the file path matches any of the exclude patterns (case-insensitive)
            for pattern in exclude {

                if path.to_string_lossy().to_ascii_lowercase().contains(pattern.to_ascii_lowercase().as_str()) {
                    // println!("Excluding file: {}", path.display());
                    return None;
                }
            }

            match file_hash(&path) {
                Ok(hash) => Some((hash, path)),
                Err(_) => None,
            }
        })
        .fold(
            || HashMap::<Vec<u8>, Vec<PathBuf>>::new(),
            |mut acc, (hash, path)| {
                // create a new entry for this hash if it doesn't exist, then push the file path into the vector
                acc.entry(hash).or_default().push(path.clone());
                acc
            },
        )
        .reduce(
            || HashMap::<Vec<u8>, Vec<PathBuf>>::new(),
            |mut a, b| {
                // merge two hash maps by extending the vectors of file paths for each hash
                for (k, v) in b {
                    a.entry(k).or_default().extend(v);
                }
                a
            },
        )
}


fn file_hash(path: &std::path::Path) -> Result<Vec<u8>> {
    let mut f = File::open(path)?;
    let mut hasher = Sha256::new();
    /*
        Create an 8 KB buffer for chunked reading.
        [0u8; 8192] = array of 8192 zeroed bytes
        We reuse this buffer for each read
        Why 8192?
        8 KB is a common efficient size
        Large enough to be fast
        Small enough to not waste memory
    */
    let mut buf = [0u8; 8192];

    // We start reading the file in chunks.
    loop {
        /*
            Read up to 8192 bytes into the buffer.
            Returns how many bytes were actually read.
            Could be less than 8192.
            Could be 0 (end of file).
        */
        let bytes_read = f.read(&mut buf)?;
        if bytes_read == 0 {
            break;
        }
        /*
            Feed the chunk into the hashing algorithm.
            We only hash the portion actually read

            Because:
                If only 3000 bytes were read,
                The rest of the buffer still contains old data.
                We must not hash garbage memory.
            This line is effectively:
            “Add this chunk to the rolling hash calculation.”
        */
        hasher.update(&buf[..bytes_read]);
    }

    Ok(hasher.finalize().to_vec())
}

pub fn delete_duplicates(duplicates: &HashMap<Vec<u8>, Vec<PathBuf>>) {
    for (hash, paths) in duplicates {
        if paths.len() > 1 {
            // We have a duplicate group
            // println!("Duplicate group (hash: {:x?}):", hash);
            // for path in paths {
            //     println!("  {}", path.display());
            // }
            // delete duplicates, e.g. keep the first one and delete the rest
            for path in &paths[1..] {
                std::fs::remove_file(path).expect(&format!("Failed to delete file: {}", path.display()));
            }
        }
    }
}