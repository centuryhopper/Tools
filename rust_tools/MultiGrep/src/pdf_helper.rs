/*

standard structure of how project will look like

*/
// fn search_files(files: Vec<PathBuf>, query: &str) {
//     files.par_iter().for_each(|path| {
//         // 1. Determine file type
//         let text = match path.extension().and_then(|s| s.to_str()) {
//             Some("pdf") => extract_pdf_text(path),
//             Some("png") | Some("jpg") => extract_image_ocr(path),
//             _ => std::fs::read_to_string(path).ok(), // Standard text
//         };
//
//         // 2. Grep the extracted text
//         if let Some(content) = text {
//             if content.contains(query) {
//                 println!("Match found in: {:?}", path);
//             }
//         }
//     });
// }
