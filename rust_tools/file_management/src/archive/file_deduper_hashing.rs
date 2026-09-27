use sha2::{Digest, Sha256};
use std::collections::HashMap;
use std::fs::File;
use std::io::{Read, Result};

/*
Big Picture: What This Function Really Does
Open file
Read file in chunks
Feed chunks into hash function
Return final hash
It never:
    Loads full file into memory
    Stores the entire file anywhere
    Cares about file type
It works for:
    Text
    Images
    Videos
    Binaries
    Anything
Because it hashes raw bytes.

Why This Is Better Than Comparing Files Directly

Instead of:

compare file A to file B
compare file A to file C
compare file B to file C


You:

compute hash for A
compute hash for B
compute hash for C
group by hash


Much faster and cleaner.

*/

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

pub fn find_duplicates_by_hashing(entries: &[std::fs::DirEntry]) -> Result<Vec<Vec<String>>> {
    let mut map: HashMap<Vec<u8>, Vec<String>> = HashMap::new();

    for entry in entries {
        let path = entry.path();
        if path.is_file() {
            let hash = file_hash(&path)?;
            map.entry(hash)
                .or_default()
                .push(path.to_string_lossy().into_owned());
        }
    }

    Ok(map.into_values().filter(|v| v.len() > 1).collect())
}
