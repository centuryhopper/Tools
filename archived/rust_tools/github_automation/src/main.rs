use std::process::Command;
use chrono::prelude::*;
use clap::Parser;
use indoc::indoc;
use std::path::Path;

///! IMPORTANT: You need to gh auth login first before using this program

/// Simple program to greet a person
#[derive(Parser, Debug)]
#[command(author, version, about, long_about = None)]
struct Args {
    /// Name of the person to greet
    #[arg(short, long)]
    name: String,

    /// Whether its a private repo
    #[arg(short, long, default_value_t = false)]
    private: bool,
}

/*
    create a private repo: target/debug/./[project_name] --name [reponame]  --private
    create a public repo: target/debug/./[project_name] --name [reponame]
*/

fn create_and_git_initialize(path: &Path, repo_name: &str)
{
    std::fs::create_dir_all(path).expect("failed to create repo path");
    std::env::set_current_dir(path).expect("failed to set current directory to the specified path");

    println!("current dir: {:?}", std::env::current_dir().unwrap());

    Command::new("git")
    .arg("init")
    .output()
    .expect("failed to initialize git in the local repo");

    Command::new("git")
    .arg("remote")
    .arg("add")
    .arg("origin")
    .arg(format!("git@github.com:leozhang1/{repo_name}.git"))
    .output()
    .expect("failed to link local repo with remote");

    std::fs::write("README.md", format!("#{repo_name}")).expect("Unable to write to readme");
    std::fs::write(".gitignore", format!(".DS_Store")).expect("Unable to write to .gitignore");

    const MIT_LICENSE : &str = indoc! {
        "
MIT License
Copyright (c) {current_year} Leo Zhang
        
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the \"Software\"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
        
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
        "
    };

    let final_text = MIT_LICENSE.replace("{current_year}", Local::now().format("%Y").to_string().as_str());

    std::fs::write("LICENSE.txt", final_text).expect("Unable to write to LICENSE.txt");

    Command::new("git")
    .arg("add")
    .arg(".")
    .output()
    .expect("failed to git add");

    Command::new("git")
    .args(["commit", "-m", "\"Initial Commit\""])
    .output()
    .expect("failed to commit");

    Command::new("git")
    .args("branch -m main".split(" "))
    .output()
    .expect("failed to rename branch to main");

    Command::new("git")
    .args("push -u origin main".split(" "))
    .output()
    .expect("failed to link local main with remote main");

    Command::new("git")
    .args("checkout -b main_laptop".split(" "))
    .output()
    .expect("failed to create main_laptop branch and move to it");

    Command::new("git")
    .args("push -u origin main_laptop".split(" "))
    .output()
    .expect("failed to link local main_laptop with remote main_laptop");

    Command::new("git")
    .args("checkout main".split(" "))
    .output()
    .expect("failed to move to main branch");
}

fn main()
{
    let args = Args::parse();
    let repo_path : String = format!("/home/{}/Documents/GitHub", std::env::var("USER").unwrap());

    if !Path::new(&repo_path).exists() {
        panic!("your base github path doesn't exist");
    }
    
    match args
    {
        Args {name: n, private: true} => {
            // println!("private repo: {n}");

            let full_path = Path::new(&repo_path).join(&n);
            
            if full_path.exists() {
                println!("{}", full_path.to_string_lossy().to_string());
                panic!("this repo already exists");
            }

            let output = Command::new("gh")
                .arg("repo")
                .arg("create")
                .arg(&n)
                .arg("--private")
                .output()
                .expect("Failed to execute gh command");

            if output.status.success() {
                let result = String::from_utf8_lossy(&output.stdout);
                println!("{}", result);
            } else {
                let error = String::from_utf8_lossy(&output.stderr);
                println!("Error: {}", error);
            }

            create_and_git_initialize(&full_path, &n);
        }

        Args {name: n, private: false} => {
            println!("public repo: {n}");

            let full_path = Path::new(&repo_path).join(&n);
            
            if full_path.exists() {
                println!("{}", full_path.to_string_lossy().to_string());
                panic!("this repo already exists");
            }

            let output = Command::new("gh")
                .arg("repo")
                .arg("create")
                .arg(&n)
                .arg("--public")
                .output()
                .expect("Failed to execute gh command");

            if output.status.success() {
                let result = String::from_utf8_lossy(&output.stdout);
                println!("{}", result);
            } else {
                let error = String::from_utf8_lossy(&output.stderr);
                println!("Error: {}", error);
            }

            create_and_git_initialize(&full_path, &n);
        }
    }
}
