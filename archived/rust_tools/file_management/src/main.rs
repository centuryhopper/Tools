mod file_utils;
use core::panic;
use indoc::indoc;
use std::env;
use std::path::Path;

fn main() {
    #![allow(unused_variables)]

    // env::set_var("RUST_BACKTRACE", "1");

    // skip the first arg which is the program name
    let args: Vec<String> = env::args().skip(1).collect();

    match args
        .iter()
        .map(|s| s.as_str())
        .collect::<Vec<&str>>()
        .as_slice()
    {
        ["cp", path1, path2] => {
            file_utils::copy_over_file(path1, path2).expect("");
        }

        ["cpdir", path1, path2] => {
            file_utils::copy_directory(&Path::new(path1), &Path::new(path2)).expect("");
        }

        ["search", path1, path2] => {
            // test example: cargo run search hyprland.conf /home/leo_zhang
            // .as_str() converts &String to &str
            file_utils::search_file(path1, path2);
        }
        ["tidy", path] => {
            // test example: cargo run /home/leo_zhang/Documents/GitHub/Tools/rust_tools/file_management/test
            let res = file_utils::organize_files_into_folders(path, &[], &[]).expect("");
        }
        _ => {
            panic!(indoc! {
                        "Error!

                    Invalid format!
                    
                    Valid format options:
                        - tidy path/to/file
                        - search path/to/file path/to/dir
                        - cp path/to/source path/to/destination
                    "
            });
        }
    }
}
