mod file_utils;
use std::env;


fn main()
{
    #![allow(unused_variables)]

    let args: Vec<String> = env::args().collect();

    println!("The program name is: {}", args[0]);

    if args.len() > 1 {
        println!("The first argument is: {}", args[1]);
    }

    file_utils::search_file(&args[1], &args[2]);

}

