use walkdir::WalkDir;
use std::fs;
use std::path::{Path};
use std::io::{self, ErrorKind};

pub fn search_file(filename: &str, dirname: &str) -> ()
{
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
    }
}


pub fn organize_files_into_folders(path: &str, extensions_to_skip: &[&str], only_target_these_extensions: &[&str]) -> io::Result<String> {

    let path = Path::new(path);
    if !path.exists()
    {
        let msg = format!("path you provided: {:?} doesn't exist!", path.as_os_str());
        return Err(std::io::Error::new(ErrorKind::Other, msg));
    }

    let files = fs::read_dir(path)?;

    for file in files {
        let file = file?;
        let file_path = file.path();
        
        if !file_path.is_file() {
            continue;
        }
        
        let file_name = file_path.file_name().unwrap().to_str().unwrap().to_owned();
        
        if !extensions_to_skip.is_empty() && extensions_to_skip.iter().any(|&ext_to_skip| file_name.ends_with(ext_to_skip))
        {
            continue;
        }
        
        if !only_target_these_extensions.is_empty() && only_target_these_extensions.iter().all(|&target_ext| !file_name.ends_with(target_ext))
        {
            continue;
        }

        let parts: Vec<&str> = file_name.rsplitn(2, ".").collect();
        println!("parts: {:?}", parts);
        if let [ext, _] = parts.as_slice() {

            let destination_folder = path.join(ext);
            let destination_file_path = destination_folder.join(&file_name);

            println!("{:?}", vec![destination_folder.as_os_str(), destination_file_path.as_os_str()]);
            println!();

            if !destination_folder.exists()
            {
                println!("{} does not exist, so making a directory for it now", destination_folder.display());
                fs::create_dir(&destination_folder)?;
            }

            // move the file
            fs::rename(&file_path, &destination_file_path)?;
        }
    }

    Ok(format!("successfully organized the files!"))
}




