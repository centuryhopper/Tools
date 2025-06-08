# Makefile

# Paths
RAYLIB_SRC = /home/leo_zhang/raylib/src
RAYLIB_WEB_LIB = $(RAYLIB_SRC)/web/libraylib.web.a

# Tools
CC = gcc
EMCC = emcc

# Flags
CFLAGS_NATIVE = -Wall -std=c99 -I$(RAYLIB_SRC)
LIBS_NATIVE = -lraylib -lm -ldl -lpthread -lGL -lrt -lX11 -Wl,-rpath=/usr/local/lib

CFLAGS_WEB = -Wall -std=c99 -Os -I$(RAYLIB_SRC) -L$(RAYLIB_SRC) -DPLATFORM_WEB \
  -s USE_GLFW=3 \
  -s ASYNCIFY \
  -s TOTAL_MEMORY=67108864 \
  -s FORCE_FILESYSTEM=1 \
  -s 'EXPORTED_FUNCTIONS=["_free","_malloc","_main"]' \
  -s EXPORTED_RUNTIME_METHODS=ccall

# Structure
SRC_DIR = src
BUILD_DIR = build
OUTPUT = output
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
TARGET = $(OUTPUT)/main

# Build all
all: native

# Build native
native: $(TARGET)
	./$(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS_NATIVE) -c $< -o $@

$(TARGET): $(OBJ) | $(OUTPUT)
	$(CC) $(OBJ) -o $@ $(LIBS_NATIVE)

# Run native
run: native
	$(TARGET)

# Valgrind
sanity: native
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(TARGET)

# Web build with Emscripten
web: | $(OUTPUT)
	$(EMCC) $(SRC) -o $(OUTPUT)/main.html $(CFLAGS_WEB) $(RAYLIB_WEB_LIB)

# Clean
clean:
	rm -f $(BUILD_DIR)/* $(OUTPUT)/*
