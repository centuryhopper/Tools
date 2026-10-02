# CPPGEN

A lightweight C++ project generator and helper that creates a clean, reusable **CMake-based C++ project structure** and keeps it in sync as the project grows.

`cppgen` automates the repetitive setup required when starting a new C++ project by generating:

- CMake configuration and presets
- Source directory structure
- Header directory structure
- Makefile
- Main source file
- Git configuration
- Optional test setup
- Project documentation

Once a project exists, `cppgen` can also **add and remove translation units** (a `.cpp` source and its matching `.hpp` header). It creates or deletes the files and regenerates the project's CMake source list, so you never hand-edit a list of `.cpp` files again.

The project uses **Typer** for the CLI interface and **Jinja2** templates for file generation.

---

# Features

- Generate modern C++ projects instantly
- Add a header/source pair with one command, including nested units like `net/client`
- Remove units safely: confirmation prompt, a check for files that still `#include` the header, and git-aware deletion
- CMake source list generated automatically from the `.cpp` files in `src/`
- `sync` command to pick up files you created or deleted by hand
- Works from any subfolder of a project, like `git`
- CMake presets (release / debug) with Ninja
- C++23 support
- Optional test setup
- Template-driven file generation
- Installable Python CLI tool
- Consistent project layout

---

# Requirements

- **cppgen:** Python 3.12+
- **Generated projects:** CMake 4.4+, Ninja, and a C++23 compiler

---

# Installation

## Install from PyPI

```bash
pip install cppgen-cli
```

or using `uv`:

```bash
uv tool install cppgen-cli
```

## Publishing (maintainers)

```bash
uv build && uv publish --token [pypi_token]
```

---

# Quick Start

Create a new C++ project:

```bash
cppgen init hello_world
cd hello_world
```

Add a translation unit:

```bash
cppgen add greeter
```

Build and run:

```bash
make run
```

Remove the unit again:

```bash
cppgen remove greeter
```

---

# CLI Usage

View available commands:

```bash
cppgen --help
```

| Command  | Description                                                       |
| -------- | ----------------------------------------------------------------- |
| `init`   | Create a reusable C++ CMake project.                              |
| `add`    | Create a header/source pair and add the source to CMake.          |
| `remove` | Delete a header/source pair and remove the source from CMake.     |
| `sync`   | Regenerate `cmake/sources.cmake` from the `.cpp` files in `src/`. |

View help for any command:

```bash
cppgen init --help
cppgen add --help
cppgen remove --help
cppgen sync --help
```

---

# Commands

## cppgen init

Creates a new C++ project.

Usage:

```bash
cppgen init [OPTIONS] PROJECT_NAME
```

Example:

```bash
cppgen init calculator
```

Creates:

```
calculator/
├── CMakeLists.txt
├── CMakePresets.json
├── Makefile
├── README.md
├── .gitignore
│
├── cmake/
│   └── sources.cmake
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

`init` fails if a directory named `PROJECT_NAME` already exists.

### Options

| Option                   | Description                             |
| ------------------------ | --------------------------------------- |
| `--tests` / `--no-tests` | Include test setup (default: `--tests`) |

#### Enable Tests

Tests are enabled by default.

```bash
cppgen init my_project
```

Generated:

```
my_project/
└── tests/
    └── test_main.cpp
```

#### Disable Tests

To generate a project without tests:

```bash
cppgen init my_project --no-tests
```

Generated:

```
my_project/
├── CMakeLists.txt
├── CMakePresets.json
├── Makefile
├── README.md
├── .gitignore
│
├── cmake/
│   └── sources.cmake
│
├── src/
│   └── main.cpp
│
└── include/
    └── my_project/
```

---

## cppgen add

Creates a header/source pair and adds the source to CMake.

Usage:

```bash
cppgen add [OPTIONS] NAME
```

Example:

```bash
cppgen add parser
```

Output:

```
created include/calculator/parser.hpp
created src/parser.cpp
updated cmake/sources.cmake
```

Resulting layout:

```
calculator/
├── cmake/
│   └── sources.cmake        # now lists src/parser.cpp
│
├── src/
│   ├── main.cpp
│   └── parser.cpp
│
└── include/
    └── calculator/
        └── parser.hpp
```

The new files start out as a minimal skeleton inside a namespace named after the project:

```cpp
// include/calculator/parser.hpp
#pragma once

namespace calculator {

// parser

} // namespace calculator
```

```cpp
// src/parser.cpp
#include "../include/calculator/parser.hpp"

namespace calculator {

} // namespace calculator
```

Include the header from elsewhere in the project with:

```cpp
#include "calculator/parser.hpp"
```

### Nested units

Use a path to put a unit in a subfolder. Missing folders are created for you:

```bash
cppgen add net/client
```

```
created include/calculator/net/client.hpp
created src/net/client.cpp
updated cmake/sources.cmake
```

Include it with `#include "calculator/net/client.hpp"`.

### Name rules

- A trailing `.cpp`, `.hpp`, or `.h` is ignored, so `cppgen add parser.cpp` is the same as `cppgen add parser`.
- Names can't be empty, absolute, or contain `..`, so a unit can't escape `src/` or `include/`.
- `add` never overwrites. If either file already exists, it stops and lists what's in the way.
- In the namespace name, `-` becomes `_`, so a project named `my-app` uses `namespace my_app`.

### Options

| Option           | Description                                                      |
| ---------------- | ---------------------------------------------------------------- |
| `--project PATH` | Project root (default: search upward from the current directory) |

---

## cppgen remove

Deletes a header/source pair and removes the source from CMake.

Usage:

```bash
cppgen remove [OPTIONS] NAME
```

Example:

```bash
cppgen remove parser
```

Output:

```
Will delete:
  include/calculator/parser.hpp
  src/parser.cpp
Continue? [y/N]: y
removed include/calculator/parser.hpp
removed src/parser.cpp
updated cmake/sources.cmake
```

Nested units work the same way: `cppgen remove net/client`. Any subfolders left empty, like `src/net/`, are cleaned up too. `src/` and `include/PROJECT_NAME/` themselves are always kept.

### Safety checks

- **Confirmation:** `remove` lists what it will delete and asks before deleting anything. Pass `-y` / `--yes` to skip the prompt.
- **Still-included headers:** before deleting a header, `cppgen` searches `src/`, `include/`, and `tests/` for files that still `#include "PROJECT_NAME/NAME.hpp"` (or `<...>`). If it finds any, it lists them and stops. Pass `--force` to delete the header anyway, which will break those files.
- **`main` is protected:** `cppgen remove main` is refused, since `src/main.cpp` is the program's entry point.
- **Git-aware:** if a file is tracked by git, its deletion is also staged. To undo it:

  ```bash
  git restore --staged --worktree src/parser.cpp include/calculator/parser.hpp
  ```

### Going header-only

To delete just the `.cpp` and keep the header, use `--keep-header`:

```bash
cppgen remove parser --keep-header
```

This is useful when moving a unit to header-only. `cppgen` warns if other files include the header, because any non-inline function it declares no longer has a definition and will fail at link time.

A later `cppgen remove parser` (without the flag) deletes the leftover header.

### Options

| Option           | Description                                                      |
| ---------------- | ---------------------------------------------------------------- |
| `--keep-header`  | Delete only the `.cpp` and keep the header                       |
| `--force`        | Delete the header even if other files include it                 |
| `-y`, `--yes`    | Don't ask for confirmation                                       |
| `--project PATH` | Project root (default: search upward from the current directory) |

---

## cppgen sync

Regenerates `cmake/sources.cmake` from the `.cpp` files in `src/`.

Usage:

```bash
cppgen sync [OPTIONS]
```

`add` and `remove` already sync for you. Run `sync` yourself after creating, deleting, or moving `.cpp` files by hand (or after switching git branches):

```bash
cppgen sync
```

Output is either `updated cmake/sources.cmake` or `already up to date`.

### Options

| Option           | Description                                                      |
| ---------------- | ---------------------------------------------------------------- |
| `--project PATH` | Project root (default: search upward from the current directory) |

---

# Working Inside a Project

`add`, `remove`, and `sync` find the project the same way `git` finds a repository: they search upward from the current directory for a folder containing both `CMakeLists.txt` and `cmake/sources.cmake`. You can run them from the project root or any subfolder. To point at a project elsewhere, use `--project`:

```bash
cppgen add parser --project ~/code/calculator
```

> **Note:** `cppgen` uses the **project folder's name** as the project name. Headers go in `include/<folder name>/`, and the folder name sets the C++ namespace. Keep the folder name the same as the name you passed to `cppgen init`. If you clone or rename the project into a folder with a different name, new headers will land in the wrong place.

---

# How the CMake Source List Works

`cppgen` keeps the list of sources in a separate generated file, `cmake/sources.cmake`, which `CMakeLists.txt` pulls in:

```cmake
add_executable(${PROJECT_NAME})

include(cmake/sources.cmake)   # generated by cppgen-cli
```

`cmake/sources.cmake` lists every `.cpp` file under `src/`, sorted:

```cmake
# generated by cppgen-cli, do not edit
target_sources(${PROJECT_NAME} PRIVATE
    src/main.cpp
    src/net/client.cpp
    src/parser.cpp
)
```

The file is never edited line by line. Each `add`, `remove`, or `sync` scans `src/` and regenerates it in full. That means:

- **It's self-healing.** Files you add or delete by hand are picked up on the next `cppgen` command.
- **It's idempotent.** Running `sync` twice changes nothing.
- **It doesn't force needless rebuilds.** The file is only rewritten when its contents actually change, so CMake only reconfigures when needed.
- **Sorted output** keeps git diffs clean.

Keep these guidelines in mind:

- **Don't edit `cmake/sources.cmake` by hand.** Your changes will be overwritten.
- **Keep the `include(cmake/sources.cmake)` line** in `CMakeLists.txt`. Everything else in `CMakeLists.txt` is yours to edit.
- **Every `.cpp` under `src/` is compiled into the main executable.** To exclude a file, move it out of `src/`.
- Third-party libraries (`find_package`, `target_link_libraries`, etc.) are not managed by `cppgen`. Add those to `CMakeLists.txt` by hand.

You don't need to re-run CMake yourself after `add` or `remove`. CMake notices that `cmake/sources.cmake` changed and reconfigures on the next build.

---

# Building Generated Projects

After creating a project:

```bash
cd PROJECT_NAME
```

Generated projects come with a `CMakePresets.json` that defines `release` and `debug` presets. Both use the Ninja generator and build into `build/<preset>/`.

---

# Build With Make

The generated `Makefile` wraps the CMake presets. From the project root:

Build:

```bash
make
```

Run:

```bash
make run
```

Pass arguments to the program:

```bash
make run ARGS="--flag value"
```

Run tests (if tests were enabled):

```bash
make test
```

Clean:

```bash
make clean
```

Everything uses the `release` preset by default. Pick another preset with `PRESET`:

```bash
make run PRESET=debug
```

`make` only re-runs CMake's configure step when the build directory is missing or `CMakePresets.json` / `CMakeUserPresets.json` changed.

---

# Build With CMake

Configure:

```bash
cmake --preset release
```

Build:

```bash
cmake --build --preset release
```

Run:

```bash
./build/release/bin/PROJECT_NAME
```

Use `debug` instead of `release` for a debug build.

---

# Running Tests

If tests were enabled:

```bash
make test
```

or with CMake directly:

```bash
cmake --preset release
cmake --build --preset release
ctest --preset release
```

Failing tests print their output.

> **Note:** The `tests` executable only builds `tests/test_main.cpp`. Units you create with `cppgen add` are compiled into the main executable, not the tests. To test one, add its `.cpp` to the `tests` target in `CMakeLists.txt`.

---

# Generated Project Structure

Every generated project follows this layout:

```
PROJECT_NAME/
│
├── CMakeLists.txt
│
├── CMakePresets.json
│
├── Makefile
│
├── README.md
│
├── .gitignore
│
├── cmake/
│   └── sources.cmake       (generated, do not edit)
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

Units added with `cppgen add NAME` go in `src/NAME.cpp` and `include/PROJECT_NAME/NAME.hpp`.

---

# Template System

`cppgen` uses Jinja2 templates to generate files.

Templates are stored inside:

```
cppgen/
└── templates/
    ├── CMakeLists.txt.j2
    ├── CMakePresets.json.j2
    ├── Makefile.j2
    ├── main.cpp.j2
    ├── README.md.j2
    ├── .gitignore.j2
    ├── test_main.cpp.j2
    ├── header.hpp.j2       (used by cppgen add)
    ├── source.cpp.j2       (used by cppgen add)
    └── sources.cmake.j2    (used by init, add, remove, and sync)
```

---

# Template Variables

Templates used by `cppgen init` have access to:

| Variable       | Description          | Example      |
| -------------- | -------------------- | ------------ |
| `project_name` | Project name         | `calculator` |
| `cpp_standard` | C++ standard version | `23`         |
| `enable_tests` | Enable tests         | `true`       |

Templates used by `cppgen add`:

| Template        | Variable    | Description                           | Example                     |
| --------------- | ----------- | ------------------------------------- | --------------------------- |
| `header.hpp.j2` | `name`      | Unit's file name, without folders     | `client`                    |
| `header.hpp.j2` | `namespace` | Project name with `-` replaced by `_` | `calculator`                |
| `source.cpp.j2` | `header`    | Header path relative to `include/`    | `calculator/net/client.hpp` |
| `source.cpp.j2` | `namespace` | Project name with `-` replaced by `_` | `calculator`                |

`sources.cmake.j2` receives `sources`, the sorted list of `.cpp` paths relative to the project root.

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

and `templates/header.hpp.j2` / `templates/source.cpp.j2` control what every `cppgen add` produces.

Changing a template changes all future generated files.

If you edit `CMakeLists.txt.j2`, keep the `include(cmake/sources.cmake)` line after `add_executable(...)` so `add`, `remove`, and `sync` keep working.

---

# Example Workflow

Create a project:

```bash
cppgen init algorithms
cd algorithms
```

Add some units:

```bash
cppgen add sorting
cppgen add graph/dijkstra
```

Build and run:

```bash
make run
```

Changed your mind about one of them?

```bash
cppgen remove graph/dijkstra
```

Created a file by hand instead of using `add`?

```bash
touch src/scratch.cpp
cppgen sync
```

---

# Why cppgen?

Starting and maintaining a C++ project usually requires repeating the same work:

- Creating CMake files
- Creating source directories
- Creating header directories
- Setting up tests
- Configuring build scripts
- Creating matching `.cpp` / `.hpp` pairs
- Keeping the CMake source list up to date

`cppgen` handles all of it:

```bash
cppgen init my_project
cppgen add my_module
```

and gives you a ready-to-build C++ project immediately.

---

# License

MIT License
