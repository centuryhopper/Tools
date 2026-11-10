# Sorting Algorithms Visualizer

Visualizes sorting algorithms with raylib + raygui. Dependencies come from vcpkg; nothing third-party is checked in.

## Setup (once)

1. Install vcpkg and set `VCPKG_ROOT` (fish):
   ```fish
   git clone https://github.com/microsoft/vcpkg ~/vcpkg
   ~/vcpkg/bootstrap-vcpkg.sh
   set -Ux VCPKG_ROOT ~/vcpkg
   fish_add_path ~/vcpkg
   ```
2. Linux only — headers raylib needs:
   ```fish
   sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl-dev
   ```

The first configure downloads and builds raylib and raygui (via vcpkg) and Unity (via CMake FetchContent) into `build/`.

## Build & run

Shortcuts: `make` (debug + run), `make asan`, `make tsan`, `make test`, `make test-asan`, `make serve`, `make clean` — `make help` lists them all. The full commands are below.

| Preset    | What it is                    |
|-----------|-------------------------------|
| `debug`   | Debug build + tests           |
| `release` | Optimized, no tests           |
| `asan`    | Debug + AddressSanitizer + UBSan + tests |
| `tsan`    | Debug + ThreadSanitizer + tests |
| `web`     | Emscripten / WebAssembly      |

```fish
cmake --preset debug
cmake --build --preset debug
./build/debug/SortingAlgorithmsVisualizer
ctest --preset debug          # run unit tests
```

Swap `debug` for `asan` or `tsan` to run the same thing under sanitizers.

## Sanitizers

```fish
make asan          # run under AddressSanitizer + UBSan, with leak checking
make asan LEAKS=0  # same, without leak checking
make tsan          # run under ThreadSanitizer (data races)
make test-asan     # unit tests under ASan + UBSan
```

If a sanitizer catches a bug, the program stops and prints the error with a stack trace pointing at the file and line.

- GPU drivers (NVIDIA, Mesa) keep some memory until exit, which LeakSanitizer would report. `lsan.supp` filters those out, so any leak that is still reported comes from our code. If a new driver library shows up in a report, add a `leak:<library name>` line to `lsan.supp`.
- ASan and TSan can't be combined, so they're separate builds.

## Web build

Needs [emsdk](https://emscripten.org/docs/getting_started/downloads.html) installed and activated:

```fish
source ~/emsdk/emsdk_env.fish
cmake --preset web
cmake --build --preset web
python3 -m http.server -d build/web
```

Then open http://localhost:8000/SortingAlgorithmsVisualizer.html

## Tests

Each `tests/src/test_*.c` file is built as its own test executable using the [Unity](https://github.com/ThrowTheSwitch/Unity) framework, and registered with CTest.
