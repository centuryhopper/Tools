use chrono::format;
use rayon::prelude::*;
use sha2::{Digest, Sha256};
use std::collections::HashMap;
use std::fs::File;
use std::io::{Read, Result, Seek, SeekFrom};
use std::path::{Path, PathBuf};
use walkdir::WalkDir;

// sequentially find all the files by their absolute paths in the current directory and its subdirectories, compute their hashes in parallel, and group them by hash to find duplicates.

fn full_hash(path: &Path) -> std::io::Result<blake3::Hash> {
    let file = File::open(path)?;

    let mut hasher = blake3::Hasher::new();

    hasher.update_reader(file)?;

    Ok(hasher.finalize())
}

/// Read the first and last portion of the file and return its hash (effective for very large files since we can return early if two very large files are not duplicates of each other)
fn partial_hash(path: &Path) -> std::io::Result<blake3::Hash> {
    const CHUNK_SIZE: usize = 64 * 1024;

    let mut file = File::open(path)?;
    let file_size = file.metadata()?.len();
    let mut hasher = blake3::Hasher::new();

    // if file_size < 2 * CHUNK_SIZE then blake3 hash the whole file otherwise hash the first and last CHUNK_SIZE kb of the file
    if file_size < (CHUNK_SIZE * 2) as u64 {
        return full_hash(path);
    }

    let mut buffer = [0u8; CHUNK_SIZE];

    // read first chunk
    // let mut bytes_read = file.read(&mut buffer)?;
    file.read_exact(&mut buffer);
    hasher.update(&buffer);

    // seek to final chunk
    // read final chunk
    let start = file_size.saturating_sub(CHUNK_SIZE as u64);
    file.seek(SeekFrom::Start(start))?;
    file.read_exact(&mut buffer);
    hasher.update(&buffer);

    Ok(hasher.finalize())
}

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

pub fn get_file_hashes(
    files: &[PathBuf],
    exclude: &[String],
) -> HashMap<blake3::Hash, Vec<PathBuf>> {
    /*
        Map → compute (hash, path)
        Fold → build local HashMap
        Reduce → merge the HashMaps

        size_groups = ...
        size_groups.retain(...)

        partial_groups = ...
        partial_groups.retain(...)

        full_groups = ...
        full_groups.retain(...)

        files
        ↓
        group by file size
        ↓
        discard unique sizes
        ↓
        partial hash remaining files
        ↓
        group by partial hash
        ↓
        discard unique partial hashes
        ↓
        full hash remaining files
        ↓
        actual duplicate groups
    */
    files
        .par_iter()
        // remove all files that cannot be hashed (e.g. permission issues)
        .filter_map(|path| {
            // check if the file path matches any of the exclude patterns (case-insensitive)
            for pattern in exclude {
                if path
                    .to_string_lossy()
                    .to_ascii_lowercase()
                    .contains(pattern.to_ascii_lowercase().as_str())
                {
                    // println!("Excluding file: {}", path.display());
                    return None;
                }
            }

            // get file size
            // ok()? returns None if fails
            let file_size = path.metadata().ok()?.len();
            Some((file_size, path))

            // match file_hash(&path) {
            //     Ok(hash) => Some((hash, path)),
            //     Err(_) => None,
            // }
        })
        .fold(
            || HashMap::<u64, Vec<PathBuf>>::new(),
            |mut acc, (file_size, path)| {
                acc.entry(file_size).or_default().push(path.to_path_buf());
                acc
            },
        )
        // one global hashmap where each vector value contains duplicates
        .reduce(
            || HashMap::<u64, Vec<PathBuf>>::new(),
            |mut a, b| {
                // merge two hash maps by extending the vectors of file paths for each hash
                for (size, paths) in b {
                    a.entry(size).or_default().extend(paths);
                }
                a
            },
        )
        .into_values() // now its a vector of paths again but this time each value now has at least one duplicate
        .filter(|paths| paths.len() > 1)
        .flatten()
        .collect::<Vec<PathBuf>>()
        .into_par_iter()
        .filter_map(|path| {
            let ph = partial_hash(&path).ok()?;
            Some((ph, path))
        })
        .fold(
            || HashMap::<blake3::Hash, Vec<PathBuf>>::new(),
            |mut acc, (partial_hash, path)| {
                acc.entry(partial_hash).or_default().push(path);
                acc
            },
        )
        .reduce(
            || HashMap::<blake3::Hash, Vec<PathBuf>>::new(),
            |mut a, b| {
                // merge two hash maps by extending the vectors of file paths for each hash
                for (partial_hash, paths) in b {
                    a.entry(partial_hash).or_default().extend(paths);
                }
                a
            },
        )
        .into_values()
        .filter(|paths| paths.len() > 1)
        .flatten()
        .collect::<Vec<PathBuf>>()
        .into_par_iter()
        .filter_map(|path| {
            let ph = full_hash(&path).ok()?;
            Some((ph, path))
        })
        .fold(
            || HashMap::<blake3::Hash, Vec<PathBuf>>::new(),
            |mut acc, (full_hash, path)| {
                acc.entry(full_hash).or_default().push(path);
                acc
            },
        )
        .reduce(
            || HashMap::<blake3::Hash, Vec<PathBuf>>::new(),
            |mut a, b| {
                for (full_hash, paths) in b {
                    a.entry(full_hash).or_default().extend(paths);
                }
                a
            },
        )

    // then filter out those corresponding vectors less then 2 and then do a full blake3 hash of the remaining and then those vectors with size 2 or greater will be the duplicate arrays

    // HashMap::new()

    // .fold(
    //     || HashMap::<Vec<u8>, Vec<PathBuf>>::new(),
    //     |mut acc, (hash, path)| {
    //         // create a new entry for this hash if it doesn't exist, then push the file path into the vector
    //         acc.entry(hash).or_default().push(path.clone());
    //         acc
    //     },
    // )
    // .reduce(
    //     || HashMap::<Vec<u8>, Vec<PathBuf>>::new(),
    //     |mut a, b| {
    //         // merge two hash maps by extending the vectors of file paths for each hash
    //         for (k, v) in b {
    //             a.entry(k).or_default().extend(v);
    //         }
    //         a
    //     },
    // )
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

pub fn delete_duplicates(duplicates: &HashMap<blake3::Hash, Vec<PathBuf>>) {
    for (_, paths) in duplicates {
        if paths.len() > 1 {
            // We have a duplicate group
            // println!("Duplicate group (hash: {:x?}):", hash);
            // for path in paths {
            //     println!("  {}", path.display());
            // }
            // delete duplicates, e.g. keep the first one and delete the rest
            for path in &paths[1..] {
                std::fs::remove_file(path)
                    .expect(&format!("Failed to delete file: {}", path.display()));
            }
        }
    }
}
