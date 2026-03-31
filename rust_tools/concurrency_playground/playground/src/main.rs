use std::{sync::atomic::{AtomicBool, AtomicU64, Ordering::{Acquire, Relaxed, Release}}, thread, time::Duration};

static DATA: AtomicU64 = AtomicU64::new(0);
static READY: AtomicBool = AtomicBool::new(false);


fn main() {
    thread::spawn(|| {
        DATA.store(234, Relaxed);
        READY.store(true, Release);
    });

    while !READY.load(Acquire) {
        thread::sleep(Duration::from_millis(1000));
        println!("Waiting for data...");
    }

    println!("Data: {}", DATA.load(Relaxed));
}
