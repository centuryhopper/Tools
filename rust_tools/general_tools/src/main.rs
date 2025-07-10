use std::{env, error::Error, io::Write};
// use uber_calculator::{Earnings};
// use keylogger::{keylog};
use autoclicker::run_autoclicker;
use donut::donut;
use keylogger::keylog;
use uber_calculator::Earnings;

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

fn main() -> Result<(), Box<dyn Error>> {
    // #[cfg(target_os = "linux")]
    // linux_specific();

    // #[cfg(target_os = "windows")]
    // windows_specific();

    // #[cfg(target_os = "macos")]
    // macos_specific();

    // println!("Platform-specific code executed.");

    let mut input = "".to_string();
    println!("Pick a tool to use.");
    println!("1 for auto clicker, 2 for donut, 3 for keylogger, 4 for uber calculator: ");
    std::io::stdout().flush().unwrap();
    std::io::stdin().read_line(&mut input)?;
    while !["1", "2", "3", "4"].contains(&input.trim()) {
        println!("1 for auto clicker, 2 for donut, 3 for keylogger, 4 for uber calculator: ");
        std::io::stdout().flush().unwrap();
        input.clear(); // clear previous input
        std::io::stdin().read_line(&mut input)?;
    }

    match input.trim() {
        "1" => {
            run_autoclicker();
        }
        "2" => {
            donut();
        }
        "3" => {
            keylog();
        }
        "4" => {
            // mileage per gallon
            // let car_mpg = args[0].parse::<u32>().unwrap();
            let mut mpg = "".to_string();
            println!("Enter the number of miles per gallon your vehicle consumes: ");
            std::io::stdout().flush().unwrap();
            std::io::stdin().read_line(&mut mpg)?;
            while mpg.trim().parse::<u32>().is_err() {
                println!("Enter the number of miles per gallon your vehicle consumes: ");
                std::io::stdout().flush().unwrap();
                mpg.clear(); // clear previous input
                std::io::stdin().read_line(&mut mpg)?;
            }
            let earnings =
                Earnings::calculate_net_uber_delivery_earnings(&mpg.trim().parse::<u32>().unwrap());

            println!(
                "(weekly net earnings: ${:.2}, monthly net earnings: ${:.2}, annual net earnings: ${:.2})",
                earnings.weekly_earnings, earnings.monthly_earnings, earnings.annual_earnings
            );
        }
        _ => return Err("Something went wrong...".into()),
    }

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

    Ok(())
}
