use rdev::{simulate, Button, EventType, Key, SimulateError};
use std::sync::{
    atomic::{AtomicBool, Ordering},
    Arc,
};
use std::{thread, time::Duration};
// use rand::prelude::*;
use rand::Rng;

fn click(i: &u32) -> Result<(), SimulateError> {
    simulate(&EventType::ButtonPress(Button::Left))?;
    thread::sleep(Duration::from_millis(50));
    simulate(&EventType::ButtonRelease(Button::Left))?;
    // println!("left mouse click {i}");
    if *i % 30 == 0 {
        // Press F5
        simulate(&EventType::KeyPress(Key::F5))?;
        thread::sleep(Duration::from_millis(50));

        // Release F5
        simulate(&EventType::KeyRelease(Key::F5))?;
    }
    Ok(())
}

fn logic() {
    let clicker_is_active = Arc::new(AtomicBool::new(false));
    let clicker_is_active_clone = clicker_is_active.clone();

    // Auto clicker thread
    thread::spawn(move || {
        let mut i = 0;
        // Create a mutable random number generator (ThreadRng)
        let mut gen = rand::rng();
        loop {
            if clicker_is_active_clone.load(Ordering::Relaxed) {
                if let Err(e) = click(&i) {
                    eprintln!("Failed to simulate click: {:?}", e);
                }
                i += 1;
                thread::sleep(Duration::from_millis(gen.random_range(1000..2000) as u64));
            }
        }
    });

    // Listen for global F6 keypress on a separate thread
    let clicker_is_active_clone2 = Arc::clone(&clicker_is_active);
    thread::spawn(move || {
        rdev::listen(move |event| {
            if let rdev::EventType::KeyPress(Key::F6) = event.event_type {
                println!("F6 pressed!");
                let current_state = clicker_is_active_clone2.load(Ordering::Relaxed);
                clicker_is_active_clone2.store(!current_state, Ordering::Relaxed);
            }
        })
        .unwrap();
    });

    // Keep the main thread alive
    loop {
        thread::sleep(Duration::from_secs(1));
    }
}

pub fn run_autoclicker() {
    println!("Running auto clicker...");
    println!("Toggle [ON/OFF] with F6");
    logic();
}
