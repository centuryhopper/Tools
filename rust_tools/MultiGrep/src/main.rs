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

fn extract_image_ocr(path: &Path) -> anyhow::Result<String> {
    let mut lt = leptess::LepTess::new(None, "eng")
        .with_context(|| format!("Failed to create leptess instance"))?;
    lt.set_image(path.to_str().unwrap_or(""))?;
    let text = lt.get_utf8_text()?;
    Ok(text)
}

fn extract_pdf_text(path: &Path) -> anyhow::Result<String> {
    let bytes = std::fs::read(path.to_str().unwrap_or(""))
        .with_context(|| format!("Failed to read PDF file: {:?}", path.to_string_lossy()))?;
    let text = pdf_extract::extract_text_from_mem(&bytes)
        .with_context(|| "Failed to extract text from pdf file".to_string())?;
    Ok(text)
}

fn search_files(files: &[PathBuf], query: &str) {
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
            //println!("content: {}", content);
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

trait extras {
    fn pathbuf_contains(&self, query: &str) -> bool;
}

impl extras for PathBuf {
    fn pathbuf_contains(&self, query: &str) -> bool {
        self.file_name()
            .unwrap()
            .to_str()
            .unwrap()
            .to_lowercase()
            .contains(&query.to_lowercase())
    }
}

fn main() {
    //let mut threads = vec![];

    // /home/leo_zhang/projects/Tools/rust_tools/MultiGrep/test
    // cargo run -- -p /home/leo_zhang/Pictures/Screenshots/ -s my_file -f Screenshot

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

    if args.file.is_none() {
        search_files(&files, &args.search.unwrap());
        return;
    }

    // pulled the lowercase query out of the closure and lowercasing it exactly once before you start iterating.
    let query_lower = args.file.as_deref().unwrap_or("").to_lowercase();

    if let Some(file) = files.iter().find(|p| p.pathbuf_contains(&query_lower)) {
        search_files(&[file.to_path_buf()], &args.search.unwrap());
    } else {
        eprintln!("your file was not found :/");
    }
}

#[cfg(test)]
mod tests;
