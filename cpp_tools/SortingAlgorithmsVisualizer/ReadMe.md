# Sorting Algorithms Visualizer

Visualizes sorting algorithms with raylib + raygui. The build uses [xmake](https://xmake.io): the whole build is described in `xmake.lua`, and xmake downloads the dependencies itself. Nothing third-party is checked in.

## Setup (once)

Install xmake (2.8.5 or newer):

```fish
sudo apt install xmake
```

or, for the newest version: `curl -fsSL https://xmake.io/shget.text | bash`

The first build downloads and builds raylib, raygui and (for tests) Unity. They are cached in `~/.xmake/packages`, outside the project, and shared between projects.

## Build & run

```fish
xmake          # build
xmake run      # run
xmake test     # build and run the unit tests
```

Switch build mode with `xmake f -m <mode>`, then build and run as above:

| Mode      | What it is                               |
|-----------|------------------------------------------|
| `debug`   | Debug build (the default)                |
| `release` | Optimized build                          |
| `asan`    | Debug + AddressSanitizer + UBSan         |
| `tsan`    | Debug + ThreadSanitizer                  |

```fish
xmake f -m release
xmake
xmake run
```

Other useful commands:

```fish
xmake -r           # rebuild everything
xmake clean        # delete build output for the current mode
rm -rf build .xmake   # start completely fresh
```

## Adding files

Nothing to edit: `xmake.lua` picks up every `src/*.c`, and every `tests/src/test_*.c` becomes its own test.

## Sanitizers

```fish
xmake f -m asan
xmake run          # run under AddressSanitizer + UBSan, with leak checking
xmake test         # unit tests under ASan + UBSan

ASAN_OPTIONS=detect_leaks=0 xmake run   # same, without leak checking

xmake f -m tsan
setarch (uname -m) -R xmake run         # run under ThreadSanitizer
```

If a sanitizer catches a bug, the program stops and prints the error with a stack trace pointing at the file and line.

- Only this project's code is instrumented; raylib, raygui and Unity are not.
- GPU drivers (NVIDIA, Mesa) keep some memory until exit, which LeakSanitizer would report. `lsan.supp` filters those out, so any leak that is still reported comes from our code. If a new driver library shows up in a report, add a `leak:<library name>` line to `lsan.supp`.
- ASan and TSan can't be combined, so they're separate modes.
- `setarch ... -R` turns off address randomization, which ThreadSanitizer needs on some kernels.

## Web build

Needs [emsdk](https://emscripten.org/docs/getting_started/downloads.html) installed and activated:

```fish
source ~/emsdk/emsdk_env.fish
xmake f -p wasm -m release
xmake
python3 -m http.server -d build/wasm/wasm32/release
```

Then open http://localhost:8000/SortingAlgorithmsVisualizer.html

To go back to the desktop build: `xmake f -p linux -m debug`.

## Tests

Each `tests/src/test_*.c` file is built as its own test executable using the [Unity](https://github.com/ThrowTheSwitch/Unity) framework, and run by `xmake test`. Use `xmake test -v` to see each test's output.
