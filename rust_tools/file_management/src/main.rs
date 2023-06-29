mod file_utils;
use std::env;




fn main()
{
    #![allow(unused_variables)]

    env::set_var("RUST_BACKTRACE", "1");

    // println!("hello");
    let args: Vec<String> = env::args().collect();

    println!("The program name is: {}", args[0]);

    if args[1..].len() == 2 {
        println!("The first argument is: {}", args[1]);
        println!("The second argument is: {}", args[2]);

        // test example: cargo run .config /home/leo_zhang
        file_utils::search_file(&args[1], &args[2]);
    }


    if args[1..].len() == 1 {
        // println!("{:?}",args[1]);

        // test example: cargo run /home/leo_zhang/Documents/GitHub/Tools/rust_tools/file_management/test
        let res = file_utils::organize_files_into_folders(&args[1], &[], &[]);

        match res
        {
            Ok(success) => {
                println!("{:?}", success)
            },

            Err(fail) => {
                println!("{:?}", fail)
            }
        }
    }
    

}

