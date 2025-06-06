#!/usr/bin/python3
# ***important that there's no space between the '#' and the '!' above***

import os
import sys
import platform
from pathlib import Path

PROJECT_NAME = "my_c_project"
try:
    PROJECT_NAME = input('Name your c project: ')
except Exception as e:
    PROJECT_NAME = "my_c_project"
Path(PROJECT_NAME).mkdir(exist_ok=True)
os.chdir(PROJECT_NAME)
SRC_DIR = "src"
BUILD_DIR = "build"
OUTPUT_DIR = "output"
INCLUDE_DIR = "include"
MAIN_FILE = os.path.join(SRC_DIR, "main.c")
MAKEFILE = "Makefile"

# Templates
MAIN_C_TEMPLATE = """#include <stdio.h>
#include "../include/example.h"

int main() {
    say_hello();
    return 0;
}
"""

EXAMPLE_H_TEMPLATE = """#ifndef EXAMPLE_H
#define EXAMPLE_H

void say_hello();

#endif
"""

EXAMPLE_C_TEMPLATE = """#include <stdio.h>
#include "../include/example.h"

void say_hello() {
    printf("Hello from example.c!\\n");
}
"""


# Cross-platform Makefile generator
def generate_makefile():
    exe_suffix = ".exe" if platform.system() == "Windows" else ""
    return f"""CC = gcc
CFLAGS = -Wall -I{INCLUDE_DIR}
BUILD_DIR = {BUILD_DIR}
OUTPUT = {OUTPUT_DIR}
SRC = $(wildcard {SRC_DIR}/*.c)
SRC_DIR = {SRC_DIR}
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
TARGET = $(OUTPUT)/{PROJECT_NAME}{exe_suffix}

all: $(TARGET)
\t./$(TARGET)

# Compile object files into build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
\t$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
\t$(CC) $(OBJ) -o $(TARGET)

sanity:                   
\tvalgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --gen-suppressions=all $(TARGET)


clean:
\trm -f $(BUILD_DIR)/* $(TARGET)
"""


def safe_write(path, content):
    if not os.path.exists(path):
        with open(path, "w") as f:
            f.write(content)


def create_project():
    print(f"Creating cross-platform C project: {PROJECT_NAME}")
    os.makedirs(SRC_DIR, exist_ok=True)
    os.makedirs(BUILD_DIR, exist_ok=True)
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    os.makedirs(INCLUDE_DIR, exist_ok=True)

    safe_write(MAIN_FILE, MAIN_C_TEMPLATE)
    safe_write(os.path.join(SRC_DIR, "example.c"), EXAMPLE_C_TEMPLATE)
    safe_write(os.path.join(INCLUDE_DIR, "example.h"), EXAMPLE_H_TEMPLATE)
    safe_write(MAKEFILE, generate_makefile())

    print("\n✅ Project created.")
    print(f"📂 {SRC_DIR}/main.c")
    print(f"📂 {SRC_DIR}/example.c")
    print(f"📂 {INCLUDE_DIR}/example.h")
    print(f"📄 {MAKEFILE}")
    print("\n💡 To build:")
    print("   On Linux/macOS: make")
    print("   On Windows with MinGW or WSL: make")
    print(f"\n🚀 To run:")
    print(f"   ./{PROJECT_NAME}{'.exe' if platform.system() == 'Windows' else ''}")


if __name__ == "__main__":
    create_project()
