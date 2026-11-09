// Fn  ⊂  FnMut  ⊂  FnOnce
// (read)  (mutate)  (consume — callable once)

// 1. Group the dependent variables together
struct State {
    stack: Vec<PathBuf>,
    active: usize, // workers currently processing a dir
    all_threads_done: bool,
}

struct Ctx {
    mtx_state: Mutex<State>,
    cvar: Condvar,
}

let ctx = Arc::new(Ctx {
        mtx_state: Mutex::new(State {
            stack: vec![root_path],
            active: 0, // number of workers currently processing a directory
            all_threads_done: false,
        }),
        cvar: Condvar::new(),
    });

    let shared_vector_of_results = Arc::new(Mutex::new(Vec::<PathBuf>::new()));

    for _ in 0..12 {
        let results_c = shared_vector_of_results.clone();
        let ctx_c = ctx.clone();

        //         it is highly recommended—and usually mandatory—to pass owned data into standard threads rather than borrowed data.
        // This requirement is strictly enforced by Rust's compiler to guarantee memory safety.
        threads.push(thread::spawn(move || {
            loop {
                let path_entry = {
                    let mut state = ctx_c
                        .cvar
                        .wait_while(ctx_c.mtx_state.lock().unwrap(), |s| {
                            s.stack.is_empty() && !s.all_threads_done
                        })
                        .unwrap();
                    if state.all_threads_done {
                        break;
                    }
                    state.active += 1;
                    state.stack.pop().unwrap()
                };

                let mut local_dirs: Vec<PathBuf> = vec![];
                let mut local_files: Vec<PathBuf> = vec![];

                if path_entry.is_symlink() {
                    continue;
                } else {
                    //   traverse direct children
                    for entry in WalkDir::new(path_entry)
                        .max_depth(1)
                        .min_depth(1)
                        .into_iter()
                        .flatten()
                    {
                        if entry.file_type().is_symlink() {
                            continue;
                        }
                        if entry.file_type().is_dir() {
                            local_dirs.push(entry.into_path());
                        } else if entry.file_type().is_file() {
                            local_files.push(entry.into_path());
                        }
                    }
                }

                results_c.lock().unwrap().extend(local_files);

                let mut state = ctx_c.mtx_state.lock().unwrap();
                let found_work = !local_dirs.is_empty();
                state.stack.extend(local_dirs);
                state.active -= 1;
                if state.stack.is_empty() && state.active == 0 {
                    state.all_threads_done = true;
                    ctx_c.cvar.notify_all();
                } else if found_work {
                    ctx_c.cvar.notify_all();
                }
            }
        }));
    }

    for th in threads {
        th.join().unwrap();
    }

    println!(
        "results count: {:?}",
        shared_vector_of_results.lock().unwrap().len()
    );

    println!("time it took: {:?}", timer.elapsed().as_secs());

    // shared_vector_of_results
    //     .lock()
    //     .unwrap()
    //     .iter()
    //     .for_each(|p| println!("{:?}", p.to_str()));
