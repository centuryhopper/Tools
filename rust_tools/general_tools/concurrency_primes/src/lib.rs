use std::f64;
use std::sync::{Arc, Mutex};
use std::thread;

const NUM_THREADS: usize = 8;
const N: usize = 100_000_000;

#[derive(Debug)]
struct SieveArray {
    size: usize,
    array: Vec<usize>,
}

#[derive(Debug)]
struct ThreadArgs {
    main_array: Arc<Mutex<Vec<bool>>>,
    start: usize,
    end: usize,
    small_primes: Arc<Vec<usize>>,
}

fn get_smallest_multiple_of_p_in_subrange(p: usize, range_start: usize) -> usize {
    (p * p).max((range_start + p - 1) / p * p)
}

fn sieve_of_eratosthenes(n: usize) -> SieveArray {
    if n > (N as f64).sqrt() as usize {
        panic!("n must be less than the square root of N");
    }

    let mut prime = vec![true; n + 1];
    prime[0] = false;
    prime[1] = false;

    for p in 2..=((n as f64).sqrt() as usize) {
        if prime[p] {
            for i in (p * p..=n).step_by(p) {
                prime[i] = false;
            }
        }
    }

    let primes: Vec<usize> = prime
        .iter()
        .enumerate()
        .filter_map(|(i, &is_prime)| if is_prime { Some(i) } else { None })
        .collect();

    SieveArray {
        size: primes.len(),
        array: primes,
    }
}

fn mark_composites(args: ThreadArgs) {
    let ThreadArgs {
        main_array,
        start,
        end,
        small_primes,
    } = args;

    let mut main_array = main_array.lock().unwrap();

    for &p in small_primes.iter() {
        let starting_point = get_smallest_multiple_of_p_in_subrange(p, start);
        for j in (starting_point..=end).step_by(p) {
            if j != p {
                main_array[j] = false;
            }
        }
    }
}

pub fn get_primes() {
    let n = (N as f64).sqrt() as usize;
    let sieve = sieve_of_eratosthenes(n);

    let main_array = Arc::new(Mutex::new(vec![true; N]));
    let small_primes = Arc::new(sieve.array);

    let range_size = N / NUM_THREADS;

    let mut handles = vec![];

    for i in 0..NUM_THREADS {
        let main_array = Arc::clone(&main_array);
        let small_primes = Arc::clone(&small_primes);

        let start = i * range_size;
        let end = if i == NUM_THREADS - 1 {
            N - 1
        } else {
            (i + 1) * range_size - 1
        };

        let args = ThreadArgs {
            main_array,
            start,
            end,
            small_primes,
        };

        handles.push(thread::spawn(move || {
            mark_composites(args);
        }));
    }

    for handle in handles {
        handle.join().unwrap();
    }

    let main_array = main_array.lock().unwrap();

    let mut cnt = 0;
    let mut sum: u64 = 0;

    for i in 2..N {
        if main_array[i] {
            cnt += 1;
            sum += i as u64;
        }
    }

    let mut largest_ten = Vec::new();
    for i in (2..N).rev() {
        if main_array[i] {
            largest_ten.push(i);
            if largest_ten.len() == 10 {
                break;
            }
        }
    }

    println!("Total number of primes: {}", cnt);
    println!("Total sum of primes: {}", sum);
    println!("Largest 10 primes less than N: {:?}", largest_ten);
}

