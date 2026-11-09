// src/tests/mod.rs
use super::*;
use std::path::PathBuf;

fn fixture(name: &str) -> PathBuf {
    PathBuf::from(env!("CARGO_MANIFEST_DIR"))
        .join("test")
        .join("fixtures")
        .join(name)
}

#[test]
fn extracts_pdf() {
    let text = extract_pdf_text(&fixture("test.pdf"))
        .unwrap_or("".into())
        .to_lowercase();

    let search_text = "redacted";
    assert!(text.contains(&search_text.to_lowercase()));
}

#[test]
fn extracts_img() {
    let text = extract_image_ocr(&fixture("test.png"))
        .unwrap_or("".into())
        .to_lowercase();
    let search_text = "rust";
    assert!(text.contains(&search_text.to_lowercase()));
}
