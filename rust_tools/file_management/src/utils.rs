
use chrono::Local;

pub fn now() -> String {
    Local::now().format("%Y/%m/%d at %H:%M:%S").to_string()
}

// println!("[{}] full hashing", now());