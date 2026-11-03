mod linked_list_recursive;

use std::{
    sync::atomic::{
        AtomicBool, AtomicU64,
        Ordering::{Acquire, Relaxed, Release},
    },
    thread,
    time::Duration,
};

static DATA: AtomicU64 = AtomicU64::new(0);
static READY: AtomicBool = AtomicBool::new(false);

fn f(num: &mut i32) {
    *num += 1;
}

fn g(num: &mut i32) {
    *num += 1;
}

fn test() {
    // thread::spawn(|| {
    //     DATA.store(234, Relaxed);
    //     READY.store(true, Release);
    // });

    // while !READY.load(Acquire) {
    //     thread::sleep(Duration::from_millis(1000));
    //     println!("Waiting for data...");
    // }

    // println!("Data: {}", DATA.load(Relaxed));

    let mut x = {
        let (a, b) = (4, 5);
        a + b
    };

    {
        let y = &x;
        println!("{}", y);
    }

    f(&mut x);
    g(&mut x);

    println!("x: {x}");
}

fn main() {}
