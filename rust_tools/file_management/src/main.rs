mod file_utils;
use core::panic;
use std::env;




fn main()
{
    #![allow(unused_variables)]

    // env::set_var("RUST_BACKTRACE", "1");
    
    // skip the first arg which is the program name
    let args: Vec<String> = env::args().skip(1).collect();
    
    match args[0].as_str() {
        "cp" => {
            if let Some(path1) = args.get(1) {
                println!("{}", path1);
                if let Some(path2) = args.get(2) {
                    println!("{}", path2);

                    file_utils::copy_over_file(path1, path2).expect("");
                }
                else { println!("need two arguments after 'cp'"); }
                
            }
            else { println!("need two arguments after 'cp'"); }
        }
        "search" => {
            if let Some(path1) = args.get(1) {
                if let Some(path2) = args.get(2) {
                    // test example: cargo run search hyprland.conf /home/leo_zhang
                    // .as_str() converts &String to &str
                    file_utils::search_file(path1.as_str(), path2.as_str());
                }
                else { println!("need two arguments after 'find'"); }
            }
            else { println!("need two arguments after 'find'"); }
        }
        "tidy" => {
            // test example: cargo run /home/leo_zhang/Documents/GitHub/Tools/rust_tools/file_management/test
            let res = file_utils::organize_files_into_folders(&args[1], &[], &[]).expect("");
        }
        _ => {
            panic!("invalid commands");
        }
    }
}

