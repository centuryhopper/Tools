use std::env;
use uber_calculator::{Earnings};
use concurrency_primes::{get_primes};
use keylogger::{keylog};


fn test(x: &str) {

    println!("{x}");
}

fn main() {

    let args: Vec<String> = env::args().skip(1).collect();

    match args.as_slice() {
        [arg] if arg.parse::<u32>().is_ok() => {
            if arg.parse::<u32>().unwrap() <= 0 
            {
                eprintln!("Please input a positve integer");
                return;
            }
            // println!("Valid u32 argument: {}", arg);
        },
        [arg] => {
            eprintln!("Error: '{}' is not a valid u32.", arg);
            return;
        },
        _ => {
            eprintln!("Error: Provide exactly one u32 argument.");
            return;
        },
    }

    // keylog();

    // get_primes();
    
    // mileage per gallon
    let car_mpg = args[0].parse::<u32>().unwrap(); 
    let earnings = Earnings::calculate_net_uber_delivery_earnings(&car_mpg);

    // let test_string = "test".to_string();
    // test(&test_string);
    // test(&test_string);

    println!(
         "(weekly net earnings: ${:.2}, monthly net earnings: ${:.2}, annual net earnings: ${:.2})",
         earnings.weekly_earnings, earnings.monthly_earnings, earnings.annual_earnings
     );
}

