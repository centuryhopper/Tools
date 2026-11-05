use std::{
    arch::x86_64,
    env,
    path::{Path, PathBuf},
    sync::{
        Arc, Condvar, Mutex,
        atomic::{AtomicUsize, Ordering},
    },
    thread,
    time::Instant,
};

use anyhow::Context;
use clap::{Parser, arg, command};
use rayon::{prelude::*, result};
use walkdir::WalkDir;

fn extract_image_ocr(path: &PathBuf) -> anyhow::Result<String> {
    let mut lt = leptess::LepTess::new(None, "eng")?;
    lt.set_image(path.to_str().unwrap_or(""))?;
    let text = lt.get_utf8_text()?;
    Ok(text)
}

fn extract_pdf_text(path: &PathBuf) -> anyhow::Result<String> {
    let bytes = std::fs::read(path.to_str().unwrap_or(""))
        .with_context(|| format!("failed to read PDF file: {:?}", path.to_string_lossy()))?;
    let text = pdf_extract::extract_text_from_mem(&bytes)?;
    Ok(text)
}

fn search_files(files: &[&PathBuf], query: &str) {
    files.par_iter().for_each(|path| {
        // 1. Determine file type
        let text = match path.extension().and_then(|s| s.to_str()) {
            Some("pdf") => extract_pdf_text(path),
            Some("png") | Some("jpg") => extract_image_ocr(path),
            _ => std::fs::read_to_string(path)
                .with_context(|| format!("failed to read text file: {}", path.display())), // Standard text
        };

        // 2. Grep the extracted text
        if let Ok(content) = text {
            // println!("content: {}", content);
            if content.to_lowercase().contains(&query.to_lowercase()) {
                println!("Match found in: {:?}", path);
            }
        }
    });
}

#[derive(Parser, Debug)]
#[command(author, version, about, long_about = None)]
struct Args {
    // file to search the text in (if this arg is not provided then all the files in the path will
    // be searched)
    #[arg(short, long)]
    file: Option<String>,

    #[arg(short, long)]
    search: Option<String>,

    #[arg(short, long)]
    path: Option<String>,
}

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

fn main() {
    //let mut threads = vec![];

    // /home/leo_zhang/projects/Tools/rust_tools/MultiGrep/test
    let args = Args::parse();

    if args.path.is_none() {
        eprintln!("The '--path' argument was not entered!");
        std::process::exit(1);
    }

    let root_path = PathBuf::from(args.path.unwrap());
    if !root_path.exists() {
        panic!("path does not exist");
    }
    if root_path.is_file() {
        panic!("please pass in a directory");
    }

    let timer = Instant::now();

    let files: Vec<PathBuf> = jwalk::WalkDir::new(&root_path)
        .skip_hidden(true)
        .into_iter()
        .flatten() // skip entries that errored
        .filter(|e| e.file_type().is_file())
        .map(|e| e.path())
        .collect();

    println!("files count: {:?}", files.len());
    println!("time it took: {:?}", timer.elapsed().as_secs());

    // pulled the lowercase query out of the closure and lowercasing it exactly once before you start iterating.
    let query_lower = args.file.as_deref().unwrap_or("").to_lowercase();

    // for i in (11000..files.len()) {
    //     println!("{:?}", files[i].as_os_str());
    // }
    //
    // // -p /home/leo_zhang/Downloads/ -s 'modern c++' -f cpp_scraping_and_philosophy_notes.txt
    // let FIND = files
    //     .iter()
    //     .find(|p| p.to_str().unwrap().to_lowercase().contains(&query_lower));

    //println!("FIND: {:?}", FIND.unwrap_or(&PathBuf::from("")).as_os_str());

    if let Some(file) = files
        .iter()
        .find(|p| p.to_str().unwrap().to_lowercase().contains(&query_lower))
    {
        search_files(&[file], &args.search.unwrap());
    } else {
        eprintln!("your file was not found :/");
    }
}
