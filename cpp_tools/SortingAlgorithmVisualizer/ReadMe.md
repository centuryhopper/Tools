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
  cd raylib/src
  mkdir web
  make -e PLATFORM=PLATFORM_WEB -B 

```
