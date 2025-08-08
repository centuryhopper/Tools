
### instructions

```
Native (Linux / Windows):
    mkdir build
    cd build
    cmake ..
    cmake --build .
    ./SortingAlgorithmsVisualizer      # Linux
    SortingAlgorithmsVisualizer.exe    # Windows

Web (Emscripten on linux):
    source ~/emsdk/emsdk_env.fish
    mkdir build_web
    emcmake cmake -B build_web -DCMAKE_BUILD_TYPE=Release
    cmake --build build_web
    # Produces SortingAlgorithmsVisualizer.html, .js, .wasm


Valgrind (Linux only):
    cmake -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build

    valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         ./build/SortingAlgorithmsVisualizer

```