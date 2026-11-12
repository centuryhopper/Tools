# data_structures

C++23 implementations of data structures: a binary search tree (`BST`) and a Fenwick tree (`FenwickTree`), with a demo program and unit tests.

## Requirements

- A C++23 compiler with `<print>` (GCC 14+ or Clang 18+)
- [xmake](https://xmake.io) 2.8.5 or newer: `curl -fsSL https://xmake.io/shget.text | bash`

## Project structure

```text
.
├── xmake.lua
├── include/data_structures/
├── src/
└── tests/
```

## Build and run

```bash
xmake          # build (debug by default)
xmake run      # run the demo program
xmake test     # build and run the unit tests
```

Switch to an optimized build with `xmake f -m release`, then build and run as above. `xmake show` prints the current mode.

New `.cpp` files in `src/` are picked up automatically.

## Clean

```bash
xmake clean            # delete build output for the current mode
rm -rf build .xmake    # start completely fresh
```

## Editor setup

`compile_commands.json` in the project root is rewritten on every build, and clangd picks it up from there automatically. Build once after cloning so the file exists.
