### instructions

```

install raylib:
  clone raylib into project root directory:
    git clone https://github.com/raysan5/raylib.git
  cd raylib/src
  sudo make

prebuild raylib to work with generating wasm files (Required if compiling your project to webassembly):
  source ~/emsdk/emsdk_env.fish # assumes you're using fish
  cp RayLibMakefile.txt raylib/src/Makefile
  mkdir -p raylib/src/web
  cd raylib/src
  emmake make clean
  compile to wasm pre-reqs:
  emmake make PLATFORM=PLATFORM_WEB USE_EVENTS=1
  compile to wasm:
    make web

  raygui:
      wget https://raw.githubusercontent.com/raysan5/raygui/master/src/raygui.h -O ./include/raygui.h

  start server:
    python3 -m http.server
    then go to http://localhost:8000/build_web/SortingAlgorithmsVisualizer.html

```
