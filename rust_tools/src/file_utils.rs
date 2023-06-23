
use std::env;
use walkdir::WalkDir;

pub fn search_file(filename: &str, dirname: &str) -> () {
    
    // no need to check current_dir for error because we know our current directory must be valid
    // let current_dir = env::current_dir().unwrap();

    for entry in WalkDir::new(dirname) {
        match entry {
            Ok(result) => {
                if result.file_name() == filename {
                    println!("Found file {} at {:?}", filename, result.path());
                }
            }

            Err(err) => {
                println!("error: {}", err);
            }
        }

        // let entry = entry.unwrap();

        // if entry.file_name() == filename {
        //     println!("Found file {} at {:?}", filename, entry.path());
        // }
    }
}


