use std::fs::{metadata, read_dir};
use std::io::{self, BufRead, BufReader, ErrorKind, Write, Error, Result};


pub fn copy_contents(src: &str, dest: &str) -> Result<String>
{
    if !metadata(src).unwrap().is_dir()
    {
        return Err(Error::new(ErrorKind::Other, format!("{:?} is not a directory", src)));
    }
    
    if !metadata(dest).unwrap().is_dir()
    {
        return Err(Error::new(ErrorKind::Other, format!("{:?} is not a directory", dest)));
    }
    
    // copy each file from src to dest unless file is already in dest
    let srcFiles = read_dir(src)?;
    let destFiles: Vec<_> = read_dir(dest)?
        .filter_map(Result::ok)
        .map(|entry| entry.path())
        .collect();

    for file in srcFiles {
        let file = file?;
        let file_path = file.path();

        if !file_path.is_file() {
            continue;
        }

        let file_name = file_path.file_name().unwrap();
        let exists_in_dest = destFiles.iter().any(|dest_path| dest_path.file_name() == Some(file_name));

        if 

        

    }


    Ok("success!".to_string())
}




fn main() {
    println!("Hello, world!");
}
