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

    with open(output_path, "w") as f:
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

    # -------------------------
    # Create directories
    # -------------------------
    (root / "src").mkdir(parents=True, exist_ok=True)
    (root / "include" / project_name).mkdir(parents=True, exist_ok=True)
    (root / "tests").mkdir(parents=True, exist_ok=True)
    (root / "build").mkdir(parents=True, exist_ok=True)

    context = {"project_name": project_name, "cpp_standard": 20, "enable_tests": tests}

    # -------------------------
    # Render templates
    # -------------------------
    render_template("CMakeLists.txt.j2", root / "CMakeLists.txt", context)

    render_template("main.cpp.j2", root / "src" / "main.cpp", context)

    if tests:
        render_template("test_main.cpp.j2", root / "tests" / "test_main.cpp", context)

    typer.secho(f"Created C++ project: {project_name}", fg=typer.colors.GREEN)


if __name__ == "__main__":
    app()
