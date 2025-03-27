// main.rs
use std::fs::{File, remove_file};
use std::thread;
use std::time::Duration;

/*
just a dummy file to test out rustc

run with rustc test.rs && ./test && rm -f ./test
*/

fn main() {
    let _f = File::create("test.txt").unwrap();
    println!("File created!");
    thread::sleep(Duration::from_secs(5));
    remove_file("test.txt").unwrap_or_else(|err| {
        println!("failed to remove test.txt");
    });
    println!("File removed!");
}
