use chrono::format;
use core::hash;
use rayon::prelude::*;
use sha2::{Digest, Sha256};
use std::collections::HashMap;
use std::fs::File;
use std::io::{Read, Result, Seek, SeekFrom};
use std::path::{Path, PathBuf};
use walkdir::WalkDir;

fn group_by<K, F>(paths: Vec<PathBuf>, functionToApplyToEachElement: F) -> HashMap<K, Vec<PathBuf>>
where
    K: Eq + std::hash::Hash + Send,
    F: Fn(&Path) -> Option<K> + Sync,
{
    let keyed: Vec<(K, PathBuf)> = paths
        .into_par_iter()
        .filter_map(|p| Some((functionToApplyToEachElement(&p)?, p)))
        .collect();

    let mut groups = HashMap::new();
    for (k, p) in keyed {
        groups.entry(k).or_insert_with(|| vec![]).push(p);
    }
    groups
}

fn keep_duplicates<K>(groups: HashMap<K, Vec<PathBuf>>) -> Vec<PathBuf> {
    groups
        .into_values()
        .filter(|v| v.len() > 1)
        .flatten()
        .collect()
}

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
    file.read_exact(&mut buffer)?;
    hasher.update(&buffer);

    // seek to final chunk
    // read final chunk
    let start = file_size.saturating_sub(CHUNK_SIZE as u64);
    file.seek(SeekFrom::Start(start))?;
    file.read_exact(&mut buffer);
    hasher.update(&buffer);

    Ok(hasher.finalize())
}

pub fn get_all_files(str_path: &str, exclude: &[String]) -> Result<Vec<PathBuf>> {
    let mut files = Vec::new();
    for entry in WalkDir::new(str_path) {
        let entry = entry?;

        let path = entry.path().to_string_lossy().to_ascii_lowercase();

        if exclude
            .iter()
            .any(|pattern| path.contains(&pattern.to_ascii_lowercase()))
        {
            continue;
        }

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
    let by_size = group_by(files.to_vec(), |p| p.metadata().ok().map(|m| m.len()));
    let by_partial = group_by(keep_duplicates(by_size), |p| partial_hash(p).ok());
    let mut by_full = group_by(keep_duplicates(by_partial), |p| full_hash(p).ok());

    by_full.retain(|_, paths| paths.len() > 1);
    by_full
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
    for (hash, paths) in duplicates {
        if paths.len() > 1 {
            println!("\nDuplicate group: {}", hash);
            println!("KEEP:   {}", paths[0].display());
            // delete duplicates, e.g. keep the first one and delete the rest
            for path in &paths[1..] {
                println!("DELETE: {}", path.display());
                std::fs::remove_file(path)
                    .expect(&format!("Failed to delete file: {}", path.display()));
            }
        }
    }
}
