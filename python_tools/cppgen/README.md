# CPPGEN

A lightweight C++ project generator that creates a clean, reusable **CMake-based C++ project structure**.

`cppgen` automates the repetitive setup required when starting a new C++ project by generating:

- CMake configuration
- Source directory structure
- Header directory structure
- Makefile
- Main source file
- Git configuration
- Optional test setup
- Project documentation

The project uses **Typer** for the CLI interface and **Jinja2** templates for file generation.

---

# Features

- Generate modern C++ projects instantly
- CMake-based build system
- C++20 support
- Optional test setup
- Template-driven file generation
- Installable Python CLI tool
- Consistent project layout

---

# Installation

## Install from PyPI

```bash
pip install cppgen
```

or using `uv`:

```bash
uv tool install cppgen
```

---

## Install From Source

Clone the repository:

```bash
git clone https://github.com/<your_username>/cppgen.git
cd cppgen
```

Install:

```bash
pip install .
```

or:

```bash
uv tool install .
```

---

# Quick Start

Create a new C++ project:

```bash
cppgen init hello_world
```

This creates:

```
hello_world/
├── CMakeLists.txt
├── Makefile
├── README.md
├── .gitignore
│
├── src/
│   └── main.cpp
│
├── include/
│   └── hello_world/
│
└── tests/
    └── test_main.cpp
```

---

# CLI Usage

View available commands:

```bash
cppgen --help
```

Example:

```
Usage: cppgen [OPTIONS] COMMAND [ARGS]...

Commands:
  init   Create a reusable C++ CMake project.
```

---

# Creating a Project

## Basic Usage

```bash
cppgen init PROJECT_NAME
```

Example:

```bash
cppgen init calculator
```

Creates:

```
calculator/
├── CMakeLists.txt
├── Makefile
├── README.md
├── .gitignore
│
├── src/
│   └── main.cpp
│
├── include/
│   └── calculator/
│
└── tests/
    └── test_main.cpp
```

---

# Commands

## cppgen init

Creates a new C++ project.

Usage:

```bash
cppgen init PROJECT_NAME
```

Example:

```bash
cppgen init data_structures
```

---

# Options

## Enable Tests

Tests are enabled by default.

Example:

```bash
cppgen init my_project
```

Generated:

```
my_project/
└── tests/
    └── test_main.cpp
```

---

## Disable Tests

To generate a project without tests:

```bash
cppgen init my_project --no-tests
```

Generated:

```
my_project/
├── CMakeLists.txt
├── Makefile
├── README.md
├── .gitignore
│
├── src/
│   └── main.cpp
│
└── include/
    └── my_project/
```

---

# Command Help

View help for the init command:

```bash
cppgen init --help
```

Example:

```
Usage: cppgen init [OPTIONS] PROJECT_NAME

Create a reusable C++ CMake project.

Arguments:
  PROJECT_NAME

Options:
  --tests / --no-tests
      Include test setup

  --help
      Show this message and exit.
```

---

# Building Generated Projects

After creating a project:

```bash
cd PROJECT_NAME
```

---

# Build With CMake

Create a build directory:

```bash
mkdir build
cd build
```

Configure:

```bash
cmake ..
```

Build:

```bash
cmake --build .
```

Run:

```bash
./PROJECT_NAME
```

---

# Build With Make

From the project root:

```bash
make
```

Run:

```bash
make run
```

Clean:

```bash
make clean
```

---

# Running Tests

If tests were enabled:

```bash
mkdir build
cd build
```

Configure and build:

```bash
cmake ..
cmake --build .
```

Run tests:

```bash
ctest
```

---

# Generated Project Structure

Every generated project follows this layout:

```
PROJECT_NAME/
│
├── CMakeLists.txt
│
├── Makefile
│
├── README.md
│
├── .gitignore
│
├── src/
│   └── main.cpp
│
├── include/
│   └── PROJECT_NAME/
│
└── tests/
    └── test_main.cpp
```

---

# Template System

`cppgen` uses Jinja2 templates to generate files.

Templates are stored inside:

```
cppgen/
└── templates/
    ├── CMakeLists.txt.j2
    ├── Makefile.j2
    ├── main.cpp.j2
    ├── README.md.j2
    ├── .gitignore.j2
    └── test_main.cpp.j2
```

---

# Template Variables

Templates have access to:

| Variable | Description | Example |
|----------|-------------|---------|
| `project_name` | Project name | `calculator` |
| `cpp_standard` | C++ standard version | `20` |
| `enable_tests` | Enable tests | `true` |

Example Jinja template:

```jinja2
project({{ project_name }})

set(CMAKE_CXX_STANDARD {{ cpp_standard }})
```

---

# Customizing Templates

You can modify the templates to match your preferred project style.

For example:

```
templates/main.cpp.j2
```

controls the generated:

```
src/main.cpp
```

Changing the template changes all future generated projects.

---

# Example Workflow

Create a project:

```bash
cppgen init algorithms
```

Enter directory:

```bash
cd algorithms
```

Build:

```bash
mkdir build
cd build

cmake ..
cmake --build .
```

Run:

```bash
./algorithms
```

---

# Development Setup

Clone the repository:

```bash
git clone https://github.com/<your_username>/cppgen.git
cd cppgen
```

Install dependencies:

```bash
uv sync
```

Run locally:

```bash
uv run cppgen init example_project
```

---

# Dependencies

- Python 3.10+
- Typer
- Jinja2

---

# Why cppgen?

Starting a C++ project usually requires repeating the same setup:

- Creating CMake files
- Creating source directories
- Creating header directories
- Setting up tests
- Configuring build scripts

`cppgen` creates this structure automatically:

```bash
cppgen init my_project
```

and gives you a ready-to-build C++ project immediately.

---

# License

MIT License