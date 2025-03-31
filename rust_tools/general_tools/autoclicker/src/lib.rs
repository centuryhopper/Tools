use std::sync::{atomic::{AtomicBool, Ordering}, Arc};
use std::{thread, time::Duration};
use rdev::{simulate, Button, EventType, Key, SimulateError};



fn click(i: &u32) -> Result<(), SimulateError> {
    simulate(&EventType::ButtonPress(Button::Left))?;
    thread::sleep(Duration::from_millis(50));
    simulate(&EventType::ButtonRelease(Button::Left))?;
    println!("left mouse click {i}");
    Ok(())
}

fn logic() {
    let clicker_is_active = Arc::new(AtomicBool::new(false));
    let clicker_is_active_clone = Arc::clone(&clicker_is_active);

    // Auto clicker thread
    thread::spawn(move || {
        let mut i = 0;
        loop {
            if clicker_is_active_clone.load(Ordering::Relaxed) {
                if let Err(e) = click(&i) {
                    eprintln!("Failed to simulate click: {:?}", e);
                }
                i+=1;
                thread::sleep(Duration::from_millis(10));
            }
        }
    });

    // Listen for global F6 keypress
    let clicker_is_active_clone2 = Arc::clone(&clicker_is_active);
    thread::spawn(move || {
        rdev::listen(move |event| {
            if let rdev::EventType::KeyPress(Key::F6) = event.event_type {
                println!("F6 pressed!");
                let current_state = clicker_is_active_clone2.load(Ordering::Relaxed);
                clicker_is_active_clone2.store(!current_state, Ordering::Relaxed);
            }
        }).unwrap();
    });

    // Keep the main thread alive
    loop {
        thread::sleep(Duration::from_secs(1));
    }
}

pub fn run_autoclicker() {
    println!("Running auto clicker...");
    logic();
}
