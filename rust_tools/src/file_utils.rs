
//use std::env;
use walkdir::WalkDir;

pub fn search_file(filename: &str, dirname: &str) {

    // let current_dir = env::current_dir().unwrap();
    for entry in WalkDir::new(dirname) {
        let entry = entry.unwrap();
        if entry.file_name() == filename {
            println!("Found file {} at {:?}", filename, entry.path());
        }
    }
}


