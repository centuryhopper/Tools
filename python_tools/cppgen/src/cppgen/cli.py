from pathlib import Path
import typer
from jinja2 import Environment, FileSystemLoader

# lets you access files inside your installed Python package
import importlib.resources as resources


app = typer.Typer()

TEMPLATE_DIR = resources.files("cppgen") / "templates"

env = Environment(
    loader=FileSystemLoader(TEMPLATE_DIR), trim_blocks=True, lstrip_blocks=True
)

init_app = typer.Typer()
app.add_typer(init_app, name="init")


def render_template(template_name: str, output_path: Path, context: dict):
    template = env.get_template(template_name)

    with open(output_path, "w", encoding="utf-8") as f:
        f.write(template.render(context))


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
    if tests:
        (root / "tests").mkdir(parents=True, exist_ok=True)
    # (root / "build").mkdir(parents=True, exist_ok=True)

    # provide variables that templates can use when generating files.
    context = {"project_name": project_name, "cpp_standard": 20, "enable_tests": tests}

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

    typer.secho(f"Created C++ project: {project_name}", fg=typer.colors.GREEN)


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