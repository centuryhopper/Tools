# GitHub Repository Creator

A small Python command-line utility that creates a new GitHub repository and automatically initializes a corresponding local Git repository.

The script uses the GitHub API through [PyGithub](https://github.com/PyGithub/PyGithub), creates a local project directory, generates a `README.md`, adds an MIT license, initializes Git, connects the local repository to GitHub, and pushes the initial commit.

## Features

* Create a GitHub repository from the command line
* Choose between public and private repositories
* Automatically create the local repository directory
* Initialize a Git repository
* Configure the GitHub remote
* Generate a `README.md`
* Generate an MIT `LICENSE.txt`
* Create a `.gitignore`
* Make an initial commit
* Rename the default branch to `main`
* Push the initial repository to GitHub

## Requirements

* Python 3
* A GitHub account
* Git
* An SSH key configured with GitHub
* A GitHub personal access token
* Python packages:

  * `PyGithub`
  * `python-dotenv`

Install the Python dependencies with:

```bash
pip install PyGithub python-dotenv
```

## Configuration

Create a `.env` file in the project directory:

```env
github_api_token=YOUR_GITHUB_TOKEN
```

The token is read using `python-dotenv` and passed to PyGithub when connecting to GitHub.

**Do not commit your `.env` file.** Add it to `.gitignore`:

```gitignore
.env
```

Your GitHub token should have the permissions necessary to create repositories.

## Usage

Run the script with the required repository name:

```bash
python create_repo.py --name my-project
```

or:

```bash
python create_repo.py -n my-project
```

### Create a private repository

Use the `--private` or `-p` flag:

```bash
python create_repo.py --name my-project --private
```

Without `--private`, the repository is created as a public repository.

## What It Does

For example:

```bash
python create_repo.py -n my-project -p
```

The script will:

1. Authenticate with GitHub using the token in `.env`.
2. Create a private GitHub repository named `my-project`.
3. Create the corresponding local directory.
4. Initialize a Git repository.
5. Create a `README.md`.
6. Create an MIT license.
7. Create a `.gitignore`.
8. Add the files to Git.
9. Create the initial commit.
10. Rename the branch to `main`.
11. Add the GitHub repository as the `origin` remote.
12. Push the `main` branch to GitHub.

Afterward, the local project will look approximately like:

```text
my-project/
├── .git/
├── .gitignore
├── LICENSE.txt
└── README.md
```

## Local Repository Location

The script currently creates repositories under:

```text
~/Documents/GitHub
```

The path is constructed dynamically using the current user's home directory:

```python
REPO_PATH = f"/home/{pwd.getpwuid(os.getuid()).pw_name}/Documents/GitHub"
```

If you want to use a different directory, modify `REPO_PATH` in the script.

## GitHub Remote

The script currently assumes the GitHub username is:

```text
centuryhopper
```

and creates the SSH remote:

```text
git@github.com:centuryhopper/<repository-name>.git
```

Change `centuryhopper` to your own GitHub username if necessary.

Your SSH key must already be configured with GitHub for the `git push` operation to work.

## Example

```bash
$ python create_repo.py -n hello-world -p

# Repository is created on GitHub
# Local repository is initialized
# Initial commit is created
# main branch is pushed to GitHub
```

You can then:

```bash
cd ~/Documents/GitHub/hello-world
git status
git remote -v
```

## Security

Never hard-code your GitHub personal access token directly into the Python source code.

Use an environment variable or `.env` file instead, and make sure `.env` is included in `.gitignore`.

If a GitHub token is accidentally committed or exposed, revoke it immediately and create a new one.

## License

This project uses the MIT License.

