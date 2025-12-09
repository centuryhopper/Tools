#!/usr/bin/env python3

import os
import sys
import json
import subprocess

PROJECT_TEMPLATE = """cmake_minimum_required(VERSION 3.10)
project({project_name} VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCV REQUIRED)

{extra_packages}

add_executable(${{PROJECT_NAME}} src/main.cpp)
target_link_libraries(${{PROJECT_NAME}} PRIVATE ${{OpenCV_LIBS}} {extra_links})
"""

MAIN_CPP_TEMPLATE = """#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::Mat image(200, 200, CV_8UC3, cv::Scalar(0, 255, 0));
    cv::imshow("Hello OpenCV", image);
    cv::waitKey(0);
    return 0;
}
"""

CONFIG_FILE = "project_config.json"


def load_config(project_name):
    file_path = os.path.join(project_name, CONFIG_FILE)
    if os.path.exists(file_path):
        with open(file_path, "r") as f:
            return json.load(f)
    return {"packages": []}


def save_config(project_name, config):
    file_path = os.path.join(project_name, CONFIG_FILE)
    with open(file_path, "w") as f:
        json.dump(config, f, indent=4)


def create_project(project_name):
    os.makedirs(project_name, exist_ok=True)
    os.makedirs(os.path.join(project_name, "src"), exist_ok=True)

    config = load_config(project_name)
    packages = config["packages"]

    extra_packages = "\n".join([f"find_package({pkg} REQUIRED)" for pkg in packages])
    extra_links = " ".join(packages)

    cmake_content = PROJECT_TEMPLATE.format(
        project_name=project_name,
        extra_packages=extra_packages,
        extra_links=extra_links,
    )

    with open(os.path.join(project_name, "CMakeLists.txt"), "w") as f:
        f.write(cmake_content)

    main_cpp = os.path.join(project_name, "src", "main.cpp")
    if not os.path.exists(main_cpp):
        with open(main_cpp, "w") as f:
            f.write(MAIN_CPP_TEMPLATE)

    print(f"✅ Project '{project_name}' created with OpenCV support.")


def add_package(project_name, package_name):
    config = load_config(project_name)
    if package_name not in config["packages"]:
        config["packages"].append(package_name)
        save_config(project_name, config)
        print(f"✅ Package '{package_name}' added.")
    else:
        print(f"⚠️ Package '{package_name}' already exists.")


def remove_package(project_name, package_name):
    config = load_config(project_name)
    if package_name in config["packages"]:
        config["packages"].remove(package_name)
        save_config(project_name, config)
        print(f"❌ Package '{package_name}' removed.")
    else:
        print(f"⚠️ Package '{package_name}' not found.")


def build_project(project_name):
    build_dir = os.path.join(project_name, "build")
    os.makedirs(build_dir, exist_ok=True)

    # Run cmake + make
    subprocess.run(["cmake", ".."], cwd=build_dir, check=True)
    subprocess.run(["cmake", "--build", "."], cwd=build_dir, check=True)
    print(f"✅ Project '{project_name}' built successfully.")


def run_project(project_name):
    build_dir = os.path.join(project_name, "build")
    exe_name = f"./{project_name}"

    if not os.path.exists(os.path.join(build_dir, project_name)):
        print("⚠️ Executable not found, building first...")
        build_project(project_name)

    subprocess.run([exe_name], cwd=build_dir)


def usage():
    print("Usage:")
    print("  python create_project.py new <project_name>   # Create new project")
    print(
        "  python create_project.py add <project_name> <package_name>   # Add extra package"
    )
    print("  python create_project.py remove <package_name># Remove package")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        usage()
        sys.exit(1)

    command = sys.argv[1]

    if command == "new":
        project_name = sys.argv[2]
        create_project(project_name)
    elif command == "add":
        project_name = sys.argv[3]
        package_name = sys.argv[2]
        add_package(project_name, package_name)
    elif command == "remove":
        project_name = sys.argv[3]
        package_name = sys.argv[2]
        remove_package(project_name, package_name)
    elif command == "build":
        project_name = sys.argv[2]
        build_project(project_name)
    elif command == "run":
        project_name = sys.argv[2]
        run_project(project_name)
    else:
        usage()
