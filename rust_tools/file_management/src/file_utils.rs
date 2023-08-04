use std::fs::{self, File};
use std::io::{self, BufRead, BufReader, ErrorKind, Write};
use std::path::Path;
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
    }
}

pub fn organize_files_into_folders(
    path: &str,
    extensions_to_skip: &[&str],
    only_target_these_extensions: &[&str],
) -> io::Result<String> {
    let path = Path::new(path);
    if !path.exists() {
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

        if !extensions_to_skip.is_empty()
            && extensions_to_skip
                .iter()
                .any(|&ext_to_skip| file_name.ends_with(ext_to_skip))
        {
            continue;
        }

        if !only_target_these_extensions.is_empty()
            && only_target_these_extensions
                .iter()
                .all(|&target_ext| !file_name.ends_with(target_ext))
        {
            continue;
        }

        // split once starting from the right and doesn't include 2, so we get the first period from the right, essentially
        let parts: Vec<&str> = file_name.rsplitn(2, ".").collect();
        println!("parts: {:?}", parts);
        if let [ext, _] = parts.as_slice() {
            let destination_folder = path.join(ext);
            let destination_file_path = destination_folder.join(&file_name);

            println!(
                "{:?}",
                vec![
                    destination_folder.as_os_str(),
                    destination_file_path.as_os_str()
                ]
            );
            println!();

            if !destination_folder.exists() {
                println!(
                    "{} does not exist, so making a directory for it now",
                    destination_folder.display()
                );
                fs::create_dir(&destination_folder)?;
            }

            // move the file
            fs::rename(&file_path, &destination_file_path)?;
        }
    }

    Ok(format!("successfully organized the files!"))
}

pub fn copy_over_file(source_file: &str, destination_file: &str) -> io::Result<()> {
    // Check if source file exists
    if !Path::new(source_file).exists() {
        println!("Source file does not exist");
        return Ok(());
    }

    // Check if destination file exists
    if Path::new(destination_file).exists() {
        // Compare the files
        if files_are_identical(source_file, destination_file)? {
            println!("Files are the same");
            return Ok(());
        }
    } else {
        if Path::new(source_file).is_dir() || Path::new(destination_file).is_dir() {
            println!(
                "Please make sure both your source and destination paths are files and not folders"
            );
            return Ok(());
        }

        // Create the destination file
        // fs::File::create(destination_file)?;
        fs::create_dir_all(Path::new(destination_file).parent().unwrap())?;

        // Create the file
        // File::create(destination_file)?;
    }

    println!("Overwriting destination file with the source");

    // Overwrite contents of destination file with those of the source
    let source = File::open(source_file)?;
    let destination = File::create(destination_file)?;

    let reader = BufReader::new(source);
    let mut writer = io::BufWriter::new(destination);

    for line in reader.lines() {
        writer.write_all(line?.as_bytes())?;
        writer.write_all(b"\n")?;
    }

    Ok(())
}

fn files_are_identical(file1: &str, file2: &str) -> io::Result<bool> {
    let contents1 = fs::read(file1)?;
    let contents2 = fs::read(file2)?;

    Ok(contents1 == contents2)
}
