use std::env;
// use uber_calculator::{Earnings};
// use keylogger::{keylog};
use autoclicker::{run_autoclicker};
use donut::{donut};


// fn test(x: &str) {
//     println!("{x}");
// }

// #[cfg(target_os = "linux")]
// fn linux_specific() {
//     println!("Running on Linux");
// }

// #[cfg(target_os = "windows")]
// fn windows_specific() {
//     println!("Running on Windows");
// }

// #[cfg(target_os = "macos")]
// fn macos_specific() {
//     println!("Running on macOS");
// }


fn main() {

    // #[cfg(target_os = "linux")]
    // linux_specific();

    // #[cfg(target_os = "windows")]
    // windows_specific();

    // #[cfg(target_os = "macos")]
    // macos_specific();

    // println!("Platform-specific code executed.");

    // donut();

    run_autoclicker();

    // let args: Vec<String> = env::args().skip(1).collect();

    // match args.as_slice() {
    //     [arg] if arg.parse::<u32>().is_ok() => {
    //         if arg.parse::<u32>().unwrap() <= 0 
    //         {
    //             eprintln!("Please input a positve integer");
    //             return;
    //         }
    //         // println!("Valid u32 argument: {}", arg);
    //     },
    //     [arg] => {
    //         eprintln!("Error: '{}' is not a valid u32.", arg);
    //         return;
    //     },
    //     _ => {
    //         eprintln!("Error: Provide exactly one u32 argument.");
    //         return;
    //     },
    // }

    // // keylog();

    // // get_primes();
    
    // // mileage per gallon
    // let car_mpg = args[0].parse::<u32>().unwrap(); 
    // let earnings = Earnings::calculate_net_uber_delivery_earnings(&car_mpg);

    // // let test_string = "test".to_string();
    // // test(&test_string);
    // // test(&test_string);

    // println!(
    //      "(weekly net earnings: ${:.2}, monthly net earnings: ${:.2}, annual net earnings: ${:.2})",
    //      earnings.weekly_earnings, earnings.monthly_earnings, earnings.annual_earnings
    //  );
}

