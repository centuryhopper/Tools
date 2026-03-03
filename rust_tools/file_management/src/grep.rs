use std::fs::{self, DirEntry};
use std::io;
use std::path::Path;

// make recursive flag optional
pub fn grep(dir: &str, target_file_name: &str, is_recursive: bool) -> io::Result<Vec<String>> {
    let root_path = Path::new(dir);
    if !root_path.try_exists()? {
        let msg = format!("The root_path: {:?} doesn't exist", root_path.as_os_str());
        // panic!("{}", msg);
        return Ok(vec![]);
    }

    let mut result = vec![];
    let mut entries: Vec<DirEntry> = fs::read_dir(root_path)?.filter_map(Result::ok).collect();

    entries.sort_by(|a, b| {
        a.file_name()
            .to_string_lossy()
            .to_lowercase()
            .cmp(&b.file_name().to_string_lossy().to_lowercase())
    });

    for entry in entries {
        let entry_full_path = entry.path();
        // println!("{:?}", entry_full_path);
        // drill into subdirectory
        if is_recursive && entry_full_path.is_dir() {
            result.extend(
                grep(entry_full_path.to_str().unwrap(), target_file_name, true)
                    .unwrap_or_else(|_| vec![]),
            );
        }

        if let Some(file_name) = entry_full_path.file_name() {
            // println!("{:?}",file_name);
            if file_name
                .to_string_lossy()
                .to_lowercase()
                .contains(&target_file_name.to_lowercase())
            {
                result.push(entry_full_path.to_string_lossy().into_owned());
            }
        }
    }

    return Ok(result);
}
