use std::process::Command;

fn main() {
    let output = Command::new("gh")
        .arg("repo")
        .arg("list")
        .output()
        .expect("Failed to execute gh command");

    if output.status.success() {
        let result = String::from_utf8_lossy(&output.stdout);
        println!("{}", result);
    } else {
        let error = String::from_utf8_lossy(&output.stderr);
        println!("Error: {}", error);
    }
}
