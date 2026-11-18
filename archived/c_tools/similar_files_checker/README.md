# similar_files_checker

A C++26 command-line tool for cleaning up photo and video backups (Synology shares, Google Takeout exports, iPhone dumps). It does two jobs:

- **Groups near-duplicates.** Burst shots, retakes and re-encoded videos are found with perceptual hashing and sorted into folders so you can review them.
- **Removes exact duplicates.** Byte-identical files are found recursively across a whole directory tree, and you choose which copy to keep with folder-priority keywords.

## How it works

### Near-duplicate images (`-i`)

1. Scan the folder for `.jpg`, `.jpeg` and `.png` files.
2. Compute a pHash for each image and read its capture time from EXIF (`DateTimeOriginal`). If there's no EXIF date, use the file's modification time.
3. Sort by timestamp and compare each image only with images taken within **30 seconds** of it, in parallel with TBB.
4. Pairs with a pHash distance of **≤ 5** are merged with union-find.
5. Each group is moved into `group_<id>/`, and singletons go to `unique_imgs/`.

### Near-duplicate videos (`-v`)

Same idea as images. The capture time comes from FFmpeg's `creation_time` tag, and the tool samples **30 frames** per video. Two videos match when **≥ 70%** of the sampled frames match. Groups go to `vid_group_<id>/`, and singletons go to `unique_vids/`.

### Exact duplicates (`-dedup`)

The tool walks the tree recursively in parallel, then narrows candidates in three passes:

1. same file size
2. same partial BLAKE3 hash
3. same full BLAKE3 hash

Duplicates are logged to `--output-path`. With `-d`, the extra copies are deleted. `--keywords` decides which copy survives: a file whose path matches an earlier keyword wins.

> **Heads up:** `-i` and `-v` **move** files (they don't copy). `-di`, `-dv` and `-dedup -d` **delete** files. Try the tool on a copy of your data first.

## Requirements

- GCC 16 (`gcc-16` / `g++-16`)
- CMake 4.3+
- Ninja
- [vcpkg](https://github.com/microsoft/vcpkg), with `VCPKG_ROOT` set

The dependencies are installed automatically by vcpkg from `vcpkg.json`: fmt, argparse, exiv2, OpenCV (with contrib, for `img_hash`), FFmpeg, oneTBB and BLAKE3.

### One-time vcpkg setup

```fish
git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
~/vcpkg/bootstrap-vcpkg.sh
set -Ux VCPKG_ROOT ~/vcpkg
fish_add_path ~/vcpkg
```

(For bash/zsh: `export VCPKG_ROOT=~/vcpkg` and `export PATH="$VCPKG_ROOT:$PATH"`.)

## Building

```sh
cmake --preset release
cmake --build --preset release
```

The first configure builds every dependency from source. It takes a while, mostly because of OpenCV and FFmpeg. Later configures reuse vcpkg's binary cache.

The binary ends up at `build/<preset>/bin/similar_files_checker`.

### Presets

| Preset                        | Purpose                               |
| ----------------------------- | ------------------------------------- |
| `release`                     | Optimized build                       |
| `debug`                       | Debug build with libstdc++ assertions |
| `debug-asan` / `release-asan` | AddressSanitizer                      |
| `debug-tsan` / `release-tsan` | ThreadSanitizer                       |

The sanitizers instrument this project's code only. The vcpkg dependencies are prebuilt without instrumentation.

### clangd

```sh
ln -sf build/debug/compile_commands.json .
```

## Usage

Pick exactly **one** mode per run.

| Flag                   | Mode                                                                                          | Required options                |
| ---------------------- | --------------------------------------------------------------------------------------------- | ------------------------------- |
| `-i`, `--images`       | Group similar images into `group_*/` folders                                                  | `--img-path`                    |
| `-v`, `--videos`       | Group similar videos into `vid_group_*/` folders                                              | `--vid-path`                    |
| `-di`, `--delete-imgs` | In each `group_*/` folder, keep one image, move it back to the parent folder, delete the rest | `--img-path`                    |
| `-dv`, `--delete-vids` | Same as `-di`, for `vid_group_*/` folders                                                     | `--vid-path`                    |
| `-l`, `--log`          | Print each file's detected capture time (US Eastern)                                          | `--img-path`, `--vid-path`      |
| `-dedup`, `--dedup`    | Find exact duplicates recursively                                                             | `--dedup-path`, `--output-path` |
| `-h`, `--help`         | Show help                                                                                     |                                 |

Options used with `-dedup`:

| Option                | Description                                                                          |
| --------------------- | ------------------------------------------------------------------------------------ |
| `-d`, `--delete`      | Actually delete the duplicates (otherwise they're only logged)                       |
| `-keys`, `--keywords` | Comma-separated folder names, highest priority first, that decide which copy to keep |

### Examples

Group burst photos, review the folders, then collapse each group to a single image:

```sh
./build/release/bin/similar_files_checker -i --img-path ~/photos/spring2024
# look through the group_* folders and delete any images you don't want kept...
./build/release/bin/similar_files_checker -di --img-path ~/photos/spring2024
```

Do the same for videos:

```sh
./build/release/bin/similar_files_checker -v  --vid-path ~/photos/spring2024
./build/release/bin/similar_files_checker -dv --vid-path ~/photos/spring2024
```

Find exact duplicates across a NAS share as a dry run, logging only:

```sh
./build/release/bin/similar_files_checker -dedup \
  --dedup-path ~/synology/root \
  --output-path ~/Downloads \
  --keywords 'iphone/iphone_11_15_2023,iphone,export'
```

Add `-d` to delete the duplicates once the log looks right.

### Converting HEIC first

HEIC images aren't scanned. To convert them to JPG first (fish, needs `libheif-examples`):

```fish
set count 0
for i in *.HEIC
    set output (string replace -r '\.HEIC$' '.jpg' "$i")
    if heif-convert "$i" "$output"
        set count (math $count + 1)
    end
end
echo "Created $count JPG files"
```

## Tests

```sh
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

The test data lives in `tests/directory_contents/`.

## Tuning

These constants are in `src/main.cpp`:

| Constant                   | Default | Meaning                                                                |
| -------------------------- | ------- | ---------------------------------------------------------------------- |
| `WINDOW`                   | 30 s    | Max time between two files for them to be compared                     |
| `TOLERABLE_PHASH_DISTANCE` | 5       | Image pHash distance: 0–2 near-identical, 3–5 strong match, 6–10 loose |
| `MATCH_RATIO_THRESHOLD`    | 0.70    | Fraction of sampled video frames that must match                       |
| `NUM_SAMPLES`              | 30      | Frames sampled per video                                               |
| `NUM_THREADS`              | 16      | Threads for the recursive directory walk in `-dedup`                   |

## Dependencies (vcpkg)

This project uses [vcpkg](https://github.com/microsoft/vcpkg) in **manifest mode** to manage C++ dependencies. `vcpkg.json` is the source of truth for what the project depends on. vcpkg installs everything automatically when CMake configures the project.

### One-time setup (Pop!_OS / Ubuntu)

1. Install the prerequisites:

   ```bash
   sudo apt update
   sudo apt install -y git curl zip unzip tar pkg-config build-essential cmake ninja-build autoconf automake libtool
   ```

2. Clone and bootstrap vcpkg:

   ```bash
   git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
   ~/vcpkg/bootstrap-vcpkg.sh -disableMetrics
   ```

3. Add vcpkg to your environment (fish shell):

   ```fish
   set -Ux VCPKG_ROOT ~/vcpkg
   fish_add_path ~/vcpkg
   ```

   Open a new terminal, then check that it works:

   ```bash
   vcpkg version
   ```

4. Update vcpkg occasionally to get newer port recipes:
   ```bash
   git -C ~/vcpkg pull
   ```

### Project setup

Create the manifest (only needed once per project):

```bash
vcpkg new --application
```

`CMakePresets.json` points CMake at the vcpkg toolchain:

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "default",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build",
      "cacheVariables": {
        "CMAKE_TOOLCHAIN_FILE": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      }
    }
  ]
}
```

### Building

```bash
cmake --preset default      # also installs any missing dependencies
cmake --build build
```

Installed packages go to `build/vcpkg_installed/x64-linux/`.

### Adding or removing a dependency

Add a dependency:

```bash
vcpkg add port fmt
```

Then add the matching `find_package` and `target_link_libraries` lines to CMake (see below).

To remove a dependency, delete its entry from the `dependencies` list in `vcpkg.json` and remove its CMake lines. (`vcpkg remove` only works in classic mode, not manifest mode.)

Search for packages:

```bash
vcpkg search <name>
```

### Finding the right CMake lines for a package

vcpkg prints the usage after each install. Many ports also save it to a file:

```fish
cat build/vcpkg_installed/x64-linux/share/<port>/usage
```

Some ports (e.g. TBB) have no `usage` file. vcpkg generates their usage at install time but doesn't save it. Find the target names in the package's CMake config instead:

```fish
ls build/vcpkg_installed/x64-linux/share/tbb/
grep -ho 'TBB::[a-z_]*' build/vcpkg_installed/x64-linux/share/tbb/*.cmake | sort -u
```

**Rule of thumb:**

- `find_package(X CONFIG REQUIRED)` with namespaced targets (`X::x`) means you only need to look up the target name.
- No `CONFIG` means an older module-style package that gives you **variables** instead of targets (e.g. OpenCV, FFmpeg). These need extra lines.

### Current dependencies

```cmake
find_package(fmt      CONFIG REQUIRED)
find_package(exiv2    CONFIG REQUIRED)
find_package(argparse CONFIG REQUIRED)
find_package(BLAKE3   CONFIG REQUIRED)
find_package(TBB      CONFIG REQUIRED)
find_package(OpenCV   REQUIRED)
find_package(FFMPEG   REQUIRED)

target_link_libraries(app PRIVATE
    fmt::fmt
    Exiv2::exiv2lib        # note: the target name differs from the package name
    argparse::argparse
    BLAKE3::blake3
    TBB::tbb
    ${OpenCV_LIBS}         # variable, not a target
)

# FFmpeg uses variables instead of targets
target_include_directories(app PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(app PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(app PRIVATE ${FFMPEG_LIBRARIES})
```

Replace `app` with your target name. Confirm the target names against each package's `usage` file, since ports can change.

OpenCV's video support needs its `ffmpeg` feature turned on in `vcpkg.json`:

```json
{ "name": "opencv4", "features": ["ffmpeg"] }
```

### Notes

- **vcpkg vs FetchContent:** vcpkg replaces FetchContent for anything it has a port for. It builds each dependency once, caches it across projects, and pins versions via `builtin-baseline` in `vcpkg.json`. FetchContent is still useful for libraries vcpkg doesn't have.
- **Versions:** the `builtin-baseline` commit pins every port's version. To upgrade dependencies, run `vcpkg x-update-baseline`, then reconfigure.
- **Cross-platform:** the same `vcpkg.json` and `CMakePresets.json` work on Windows and macOS. Only the triplet folder changes (`x64-windows`, `arm64-osx`).
