from pathlib import Path
import subprocess
from typing import NoReturn

import typer
from jinja2 import Environment, FileSystemLoader

# lets you access files inside your installed Python package
import importlib.resources as resources


app = typer.Typer()

TEMPLATE_DIR = resources.files("cppgen") / "templates"

env = Environment(
    loader=FileSystemLoader(TEMPLATE_DIR),
    trim_blocks=True,
    lstrip_blocks=True,
    # Jinja drops the final newline by default. Keeping it means a freshly
    # rendered sources.cmake is byte-for-byte identical to the one already on
    # disk when nothing changed, which is how sync_sources decides to skip
    # the write.
    keep_trailing_newline=True,
)

ctx = {
    "class_name": "MyClass",
    "namespace": "project::core",     # nested namespaces work (C++17)
    "header_include": "project/core/my_class.hpp",
}

init_app = typer.Typer()
app.add_typer(init_app, name="init")

# Location of the generated source list, relative to the C++ project root.
# cppgen owns this file and rewrites it whole; CMakeLists.txt pulls it in
# with include(cmake/sources.cmake). Nobody should edit it by hand.
SOURCES_CMAKE = Path("cmake") / "sources.cmake"

# File types scanned when checking whether anything still includes a header.
CPP_SUFFIXES = {".cpp", ".cc", ".cxx", ".hpp", ".h", ".hxx"}


def render_template(template_name: str, output_path: Path, context: dict):
    template = env.get_template(template_name)

    with open(output_path, "w", encoding="utf-8") as f:
        f.write(template.render(context))


def fail(message: str) -> NoReturn:
    """Print an error in red and exit with a non-zero status."""
    typer.secho(message, fg=typer.colors.RED, err=True)
    raise typer.Exit(code=1)


# -------------------------
# Project discovery
# -------------------------


def find_project_root(start: Path) -> Path:
    """
    Walk up from `start` until we find a cppgen project, the same way git
    looks for .git. This lets `cppgen add` work from any subfolder.

    A directory counts as a cppgen project when it has both a CMakeLists.txt
    and the generated cmake/sources.cmake (which `init` always creates).
    """
    start = start.resolve()
    for directory in (start, *start.parents):
        if (directory / "CMakeLists.txt").is_file() and (
            directory / SOURCES_CMAKE
        ).is_file():
            return directory
    fail(f"Not inside a cppgen project (no {SOURCES_CMAKE} found above {start}).")


def resolve_project(project: Path | None) -> Path:
    """Use --project if given, otherwise search upward from the cwd."""
    if project is not None:
        project = project.resolve()
        if not (project / SOURCES_CMAKE).is_file():
            fail(f"{project} is not a cppgen project (missing {SOURCES_CMAKE}).")
        return project
    return find_project_root(Path.cwd())


# -------------------------
# Unit naming and paths
# -------------------------


def normalize_unit_name(name: str) -> str:
    """
    Turn what the user typed into a clean unit name:
      "hasher.cpp"   -> "hasher"
      "net/client/"  -> "net/client"

    Rejects empty names and ".." so a unit can't escape src/ or include/.
    """
    for suffix in (".cpp", ".hpp", ".h"):
        name = name.removesuffix(suffix)
    name = name.strip("/")

    parts = Path(name).parts
    if not parts or ".." in parts or Path(name).is_absolute():
        fail(f"Invalid unit name: '{name}'")
    return Path(*parts).as_posix()


def header_root(project: Path) -> Path:
    """
    Directory that headers are created in: include/<project_name>/.

    Headers are namespaced by project, so they're included as
    "myproj/hasher.hpp", matching the folder `init` creates. The project
    name is taken from the root folder's name, which `init` sets.

    This is always the same path, even if the folder is currently empty or
    missing (git doesn't track empty folders, so a fresh clone may not have
    it). `add` creates it when needed.
    """
    return project / "include" / project.name


def unit_paths(project: Path, name: str) -> tuple[Path, Path, str]:
    """
    Where a unit's two files live, plus the string used to #include it.

    For project "myproj" and name "net/client":
      header  -> include/myproj/net/client.hpp
      source  -> src/net/client.cpp
      include -> "myproj/net/client.hpp"  (relative to include/, which is
                 the include directory CMakeLists.txt adds to the target)
    """
    header = header_root(project) / f"{name}.hpp"
    source = project / "src" / f"{name}.cpp"
    include_str = header.relative_to(project / "include").as_posix()
    return header, source, include_str


# -------------------------
# Syncing cmake/sources.cmake
# -------------------------


def sync_sources(project: Path) -> bool:
    """
    Regenerate cmake/sources.cmake from the .cpp files currently on disk.

    This never edits the file line by line. It lists every .cpp under src/,
    renders the whole file from the template, and overwrites only if the
    result differs. That makes it:
      - idempotent (running it twice changes nothing)
      - self-healing (files added/deleted by hand are picked up next run)

    Returns True if the file was rewritten.
    """
    # Sorted so the file's order is stable and git diffs stay clean.
    # gets cpp files in src so that we can update the sources.cmake file with all the cpp files in the project
    files = sorted(
        p.relative_to(project).as_posix() for p in (project / "src").rglob("*.cpp")
    )
    content = env.get_template("sources.cmake.j2").render(sources=files)

    out = project / SOURCES_CMAKE
    out.parent.mkdir(parents=True, exist_ok=True)

    # Only write on a real change: touching this file makes CMake
    # reconfigure, so skipping no-op writes keeps builds fast.
    if out.exists() and out.read_text(encoding="utf-8") == content:
        return False
    out.write_text(content, encoding="utf-8")
    return True


def report_sync(project: Path) -> None:
    """Run sync_sources and tell the user whether anything changed."""
    if sync_sources(project):
        typer.echo(f"updated {SOURCES_CMAKE}")


# -------------------------
# File helpers for add/remove
# -------------------------


def find_includers(project: Path, include_str: str, exclude: list[Path]) -> list[Path]:
    """
    Return every C++ file that still #includes the given header, in either
    the "quoted" or <angled> form. Files being deleted are excluded.

    This is a plain text search, not a real preprocessor, but it catches the
    common way a removal would break the build.
    """
    needles = (f'"{include_str}"', f"<{include_str}>")
    excluded = {p.resolve() for p in exclude}
    found = []

    for folder in ("src", "include", "tests"):
        directory = project / folder
        if not directory.is_dir():
            continue
        for path in directory.rglob("*"):
            if path.suffix not in CPP_SUFFIXES or path.resolve() in excluded:
                continue
            text = path.read_text(encoding="utf-8", errors="ignore")
            if any(needle in text for needle in needles):
                found.append(path)

    return found


def is_git_tracked(project: Path, path: Path) -> bool:
    """True if `path` is tracked by git (False if git is missing or not a repo)."""
    try:
        result = subprocess.run(
            ["git", "ls-files", "--error-unmatch", str(path)],
            cwd=project,
            capture_output=True,
        )
    except FileNotFoundError:  # git isn't installed
        return False
    return result.returncode == 0


def delete_file(project: Path, path: Path) -> None:
    """
    Delete a file. For tracked files the deletion is also staged in git, so
    it's easy to undo with `git restore --staged --worktree <file>`.

    We stage with `git rm --cached` and delete the file ourselves instead of
    using plain `git rm`, because plain `git rm` also deletes any parent
    folders it leaves empty. That would wipe out include/<project>/ when its
    last header goes. Folder cleanup is left to remove_empty_dirs, which
    knows which folders must survive.
    """
    if is_git_tracked(project, path):
        result = subprocess.run(
            ["git", "rm", "--cached", "-q", str(path)], cwd=project, check=True, text=True,
        )
        if result.returncode != 0:
            fail(f"git refused to unstage {path.relative_to(project)}:\n{result.stderr.strip()}")
    path.unlink()


def remove_empty_dirs(path: Path, stop: Path) -> None:
    """
    After deleting src/net/client.cpp, remove src/net/ if it is now empty,
    walking upward but never removing `stop` (src/ or include/<project>/)
    itself.
    """
    stop = stop.resolve()
    directory = path.parent.resolve()
    while directory != stop and stop in directory.parents:
        if not directory.exists():
            # Already gone; keep walking up in case its parent is now empty.
            directory = directory.parent
            continue
        if any(directory.iterdir()):
            break  # still has files, so it and everything above it stay
        directory.rmdir()
        directory = directory.parent


# -------------------------
# Commands
# -------------------------


# @app.command()
@init_app.callback(invoke_without_command=True)
def init(
    project_name: str, tests: bool = typer.Option(True, help="Include test setup")
):
    """
    Create a reusable C++ CMake project.
    """

    root = Path(project_name)

    if root.exists():
        typer.secho(
            f"Directory '{project_name}' already exists.",
            fg=typer.colors.RED,
        )
        raise typer.Exit(code=1)

    # -------------------------
    # Create directories
    # -------------------------
    (root / "src").mkdir(parents=True, exist_ok=True)
    (root / "include" / project_name).mkdir(parents=True, exist_ok=True)
    # Holds the generated sources.cmake that CMakeLists.txt includes.
    (root / "cmake").mkdir(parents=True, exist_ok=True)
    if tests:
        (root / "tests").mkdir(parents=True, exist_ok=True)

    # provide variables that templates can use when generating files.
    context = {"project_name": project_name, "cpp_standard": 23, "enable_tests": tests}

    templates = [
        ("CMakeLists.txt.j2", "CMakeLists.txt"),
        ("Makefile.j2", "Makefile"),
        ("main.cpp.j2", "src/main.cpp"),
        (".gitignore.j2", ".gitignore"),
        ("README.md.j2", "README.md"),
    ]

    # -------------------------
    # Render templates
    # -------------------------
    for template, output in templates:
        render_template(template, root / output, context)

    if tests:
        render_template("test_main.cpp.j2", root / "tests" / "test_main.cpp", context)
        
    # generate CMakePresets.json
    render_template("CMakePresets.json.j2", root / "CMakePresets.json", context)
    

    # Generate cmake/sources.cmake last, from whatever .cpp files now exist
    # (just src/main.cpp at this point). It is rendered by the same function
    # that `add`, `remove` and `sync` use, so the format can never drift.
    # This file also marks the directory as a cppgen project for
    # find_project_root.
    sync_sources(root)

    typer.secho(f"Created C++ project: {project_name}", fg=typer.colors.GREEN)


@app.command()
def add(
    name: str = typer.Argument(..., help="Unit name, e.g. hasher or net/client"),
    project: Path | None = typer.Option(
        None, help="Project root (default: search upward from the current directory)"
    ),
):
    """
    Create a header/source pair and add the source to CMake.
    """
    root = resolve_project(project)
    name = normalize_unit_name(name)
    header, source, include_str = unit_paths(root, name)

    # Refuse to overwrite anything that already exists.
    existing = [p for p in (header, source) if p.exists()]
    if existing:
        fail("Already exists: " + ", ".join(str(p.relative_to(root)) for p in existing))

    # Nested names like net/client need their subfolders created first.
    header.parent.mkdir(parents=True, exist_ok=True)
    source.parent.mkdir(parents=True, exist_ok=True)

    # Project folder names can contain "-", which isn't valid in a C++ identifier.
    namespace = root.name.replace("-", "_")

    render_template("header.hpp.j2", header, {"name": Path(name).name, "namespace": namespace})
    render_template("source.cpp.j2", source, {"header": include_str, "namespace": namespace})

    for path in (header, source):
        typer.echo(f"created {path.relative_to(root)}")

    # The disk changed, so bring CMake's source list up to date.
    report_sync(root)


@app.command()
def remove(
    name: str = typer.Argument(..., help="Unit name, e.g. hasher or net/client",),
    keep_header: bool = typer.Option(
        False,
        "--keep-header",
        help="Delete only the .cpp and keep the header (e.g. going header-only)",
    ),
    force: bool = typer.Option(
        False, "--force", help="Delete the header even if other files include it"
    ),
    project: Path | None = typer.Option(
        None, help="Project root (default: search upward from the current directory)"
    ),
    yes: bool = typer.Option(False, "--yes", "-y", help="Don't ask for confirmation"),
):
    """
    Delete a header/source pair and remove the source from CMake.
    """
    root = resolve_project(project)
    name = normalize_unit_name(name)

    # src/main.cpp is the program entry point and has no header; deleting it
    # through this command is almost certainly a mistake.
    if name == "main":
        fail("Refusing to remove main.")

    header, source, include_str = unit_paths(root, name)

    # Decide what to delete. --keep-header narrows the set to just the .cpp.
    if keep_header:
        if not source.exists():
            fail(f"Nothing to remove: {source.relative_to(root)} does not exist.")
        targets = [source]
    else:
        # Normal case: whichever of the pair exists. This also handles a unit
        # whose .cpp was already removed earlier with --keep-header.
        targets = [p for p in (header, source) if p.exists()]
        if not targets:
            fail(f"No files found for '{name}'.")

    # Safety check: who still includes this header?
    includers = find_includers(root, include_str, exclude=targets)
    if includers:
        listing = "\n  ".join(str(p.relative_to(root)) for p in includers)

        if keep_header:
            # The header stays, so these files still compile. But any
            # non-inline function it declares just lost its definition, so
            # calling one would fail at link time. Warn, don't block.
            typer.secho(
                f'Warning: "{include_str}" is included by:\n  {listing}\n'
                "Any non-inline functions it declares no longer have a definition.",
                fg=typer.colors.YELLOW,
            )
        elif not force:
            # The header is going away, so these files would stop compiling.
            fail(
                f'"{include_str}" is still included by:\n  {listing}\n'
                "Use --force to remove anyway."
            )
        else:
            typer.secho(
                f'Warning: removing "{include_str}" breaks:\n  {listing}',
                fg=typer.colors.YELLOW,
            )

    typer.echo("Will delete:\n  " + "\n  ".join(str(p.relative_to(root)) for p in targets))
    if not yes and not typer.confirm("Continue?"):
        raise typer.Exit()
    # Delete, then tidy up any folders a nested unit left empty.
    for path in targets:
        delete_file(root, path)
        typer.echo(f"removed {path.relative_to(root)}")
        if path == source:
            remove_empty_dirs(path, stop=root / "src")
        else:
            remove_empty_dirs(path, stop=header_root(root))

    # The disk changed, so bring CMake's source list up to date.
    report_sync(root)


@app.command()
def sync(
    project: Path | None = typer.Option(
        None, help="Project root (default: search upward from the current directory)"
    ),
):
    """
    Regenerate cmake/sources.cmake from the .cpp files in src/.

    Useful after creating or deleting files by hand.
    """
    root = resolve_project(project)
    if sync_sources(root):
        typer.echo(f"updated {SOURCES_CMAKE}")
    else:
        typer.echo("already up to date")


if __name__ == "__main__":
    app()


'''
# Publishing a Python Package to PyPI with uv

## 1. Prepare `pyproject.toml`

Make sure the package name is unique on PyPI:

```toml
[project]
name = "my-package-name"
version = "0.1.0"
description = "Description of the package"
readme = "README.md"
requires-python = ">=3.12"

dependencies = [
    "typer>=0.25.1,<0.26.0",
    "jinja2>=3.1.6,<4.0.0"
]

[project.scripts]
my-cli-command = "package_name.cli:app"

[tool.hatch.build.targets.wheel]
packages = ["src/package_name"]

[build-system]
requires = ["hatchling"]
build-backend = "hatchling.build"

[dependency-groups]
dev = [
    "build",
]
```

Notes:

* `project.name` is the name used on PyPI.
* The CLI command name can be different from the PyPI package name.
* `packages` tells Hatch where the Python package lives.
* If your package contains templates/assets, verify they are included in the wheel.

---

## 2. Add package metadata

`src/package_name/__init__.py`

```python
"""Package description."""

__version__ = "0.1.0"
```

---

## 3. Clean previous builds

Before rebuilding, remove old artifacts:

```bash
rm -rf dist/
```

This prevents accidentally uploading an old wheel with the previous package name.

---

## 4. Build the package

Install build tools:

```bash
uv add --dev build
```

Build:

```bash
uv run python -m build
```

Expected output:

```
dist/
├── my_package_name-0.1.0.tar.gz
└── my_package_name-0.1.0-py3-none-any.whl
```

---

## 5. Verify the wheel contents

List files inside the wheel:

```bash
unzip -l dist/*.whl
```

Check metadata:

```bash
unzip -p dist/*.whl '*METADATA' | grep Name
```

Expected:

```
Name: my-package-name
```

If it shows an old name, delete `dist/` and rebuild.

---

## 6. Create a PyPI API token

Go to:

https://pypi.org/manage/account/

Create an API token.

Use:

* Account-wide token for first upload.
* Project-scoped token after the project exists.

Copy the token immediately because PyPI only shows it once.

---

## 7. Publish with uv

Set your token:

Linux/macOS:

```bash
export UV_PUBLISH_TOKEN=pypi-your-token-here
```

Publish:

```bash
uv publish --token pypi-your-token-here
```

---

## 8. Test installation

After publishing:

```bash
uv tool install my-package-name
```

or:

```bash
pip install my-package-name
```

Test the CLI:

```bash
my-cli-command --help
```

---

## Common Errors

### Error: 403 Forbidden

Example:

```
The user 'username' isn't allowed to upload to project 'cppgen'
```

Cause:

* The package name already exists on PyPI.
* Or you are uploading an old wheel.

Fix:

```bash
rm -rf dist/
uv run python -m build
ls dist/
```

Verify the filename uses your new package name.

---

### Error: Templates/assets missing after installation

Check the wheel:

```bash
unzip -l dist/*.whl
```

Make sure non-Python files are included:

```
package_name/
└── templates/
    ├── template1.j2
    └── template2.j2
```

---

## Release Checklist

Before every release:

```bash
rm -rf dist/
uv add --dev build twine
uv lock
uv run python -m build
unzip -l dist/*.whl
uv publish --token pypi-your-token-here
```

After publishing:

```bash
uv tool install package-name
package-command --help
```

'''