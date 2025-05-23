# Makefile

# Paths
RAYLIB_SRC = /home/leo_zhang/raylib/src
RAYLIB_WEB_LIB = $(RAYLIB_SRC)/web/libraylib.web.a

# Compiler flags
CFLAGS_NATIVE = -Wall
LIBS_NATIVE = -lraylib -lm -ldl -lpthread -lGL -lrt -lX11 -Wl,-rpath=/usr/local/lib

CFLAGS_WEB = -Wall -std=c99 -Os \
  -I$(RAYLIB_SRC) -L$(RAYLIB_SRC) \
  $(RAYLIB_WEB_LIB) \
  -s USE_GLFW=3 \
  -s ASYNCIFY \
  -s TOTAL_MEMORY=67108864 \
  -s FORCE_FILESYSTEM=1 \
  -DPLATFORM_WEB \
  -s 'EXPORTED_FUNCTIONS=["_free","_malloc","_main"]' \
  -s EXPORTED_RUNTIME_METHODS=ccall

# Targets
all: native

native:
	gcc ./src/main.c ./src/bubble_sort.c ./src/draw_state.c -o main $(CFLAGS_NATIVE) $(LIBS_NATIVE)

run: native
	./main

web:
	emcc ./src/main.c ./src/bubble_sort.c ./src/draw_state.c -o main.html $(CFLAGS_WEB)
	mkdir -p ./wasm
	mv main.html main.js main.wasm ./wasm

clean:
	rm -f ./main
	rm -rf ./wasm
