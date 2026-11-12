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
- [xmake](https://xmake.io) 2.8.5 or newer: `curl -fsSL https://xmake.io/shget.text | bash`

xmake downloads and builds every dependency itself: fmt, argparse, exiv2, OpenCV (with contrib, for `img_hash`, and FFmpeg for video), FFmpeg, oneTBB, BLAKE3 and xxHash. They're cached in `~/.xmake/packages` and shared between projects.

## Building

```sh
xmake            # release build (the default)
xmake run similar_files_checker -h
```

The first build compiles every dependency from source. It takes a while, mostly because of OpenCV and FFmpeg. Later builds reuse them.

The binary ends up at `build/linux/x86_64/<mode>/similar_files_checker`. `xmake run` starts it in the project root, so relative paths work.

### Modes and sanitizers

Configure once with `xmake f`; every later `xmake`, `xmake run` and `xmake test` uses it.

| Configure command                          | Purpose                               |
| ------------------------------------------ | ------------------------------------- |
| `xmake f -m release`                       | Optimized build                       |
| `xmake f -m debug`                         | Debug build with libstdc++ assertions |
| `xmake f -m debug --sanitizer=address`     | AddressSanitizer (any mode)           |
| `xmake f -m debug --sanitizer=thread`      | ThreadSanitizer (any mode)            |
| `xmake f --sanitizer=none`                 | Back to no sanitizer                  |

The sanitizers instrument this project's code only, not the dependencies. ThreadSanitizer can abort at startup on newer kernels ("unexpected memory mapping"); run it with address randomization off:

```fish
setarch (uname -m) -R xmake run similar_files_checker <args>
```

`xmake show` prints the current mode. `rm -rf build .xmake` starts completely fresh.

### clangd

`compile_commands.json` in the project root is rewritten on every build, so clangd works without extra steps.

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
xmake run similar_files_checker -i --img-path ~/photos/spring2024
# look through the group_* folders and delete any images you don't want kept...
xmake run similar_files_checker -di --img-path ~/photos/spring2024
```

Do the same for videos:

```sh
xmake run similar_files_checker -v  --vid-path ~/photos/spring2024
xmake run similar_files_checker -dv --vid-path ~/photos/spring2024
```

Find exact duplicates across a NAS share as a dry run, logging only:

```sh
xmake run similar_files_checker -dedup \
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
xmake test        # builds the tests target and runs it
xmake test -v     # same, showing the test's output
```

The test data lives in `tests/directory_contents/`; its absolute path is passed to the tests as `TEST_DATA_DIR`.

## Tuning

These constants are in `src/main.cpp`:

| Constant                   | Default | Meaning                                                                |
| -------------------------- | ------- | ---------------------------------------------------------------------- |
| `WINDOW`                   | 30 s    | Max time between two files for them to be compared                     |
| `TOLERABLE_PHASH_DISTANCE` | 5       | Image pHash distance: 0–2 near-identical, 3–5 strong match, 6–10 loose |
| `MATCH_RATIO_THRESHOLD`    | 0.70    | Fraction of sampled video frames that must match                       |
| `NUM_SAMPLES`              | 30      | Frames sampled per video                                               |
| `NUM_THREADS`              | 16      | Threads for the recursive directory walk in `-dedup`                   |

## Dependencies (xmake)

Dependencies are declared in `xmake.lua` and installed automatically on the first build.

```lua
add_requires("fmt", "exiv2", "argparse", "blake3", "tbb", "ffmpeg", "xxhash")
add_requires("opencv", {configs = {ffmpeg = true}})

add_packages("fmt", "exiv2", "argparse", "blake3", "tbb", "ffmpeg", "xxhash", "opencv")
```

`add_requires` downloads and builds a package; `add_packages` adds its include paths, libraries and link flags to the targets. Unlike CMake, there are no per-package target names or variables to look up.

### Adding or removing a dependency

1. Find the package's exact name: `xrepo search <name>`.
2. See its versions and build options: `xrepo info <name>`.
3. Add it to both `add_requires` and `add_packages` in `xmake.lua`, then run `xmake`.

To remove one, delete it from both lines.

### Versions

Installed packages never upgrade on their own. To upgrade:

```sh
xmake require --upgrade
```

To pin a version, put it in the name: `add_requires("fmt 11.x")`.
