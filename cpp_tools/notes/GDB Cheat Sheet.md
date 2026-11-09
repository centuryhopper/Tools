# GDB Cheat Sheet

Oct 8, 2026 · @lynn

## Setup

gdb needs a build with debug info (`-g`); without it you get addresses instead of line numbers and no variable names.

- Build: `make build PRESET=debug`, or `cmake --preset debug` then `cmake --build --preset debug`.
- Check: `file build/debug/bin/tests` should say `with debug_info`.
- Prefer `Debug` over `RelWithDebInfo` for stepping: `RelWithDebInfo` defines `NDEBUG` (asserts vanish) and optimizes some variables to `<optimized out>`.

| Command | What it does |
| --- | --- |
| `gdb ./build/debug/bin/tests` | Load a program |
| `gdb --args ./prog --flag value` | Load a program with arguments |
| `sudo gdb -p <pid>` | Attach to an already-running process |
| `file <path>` | Switch to a different binary inside gdb |
| `quit` (`q`) | Exit gdb |

## Running and stopping

`run` and `start` always begin from scratch; breakpoints survive restarts.

| Command | What it does |
| --- | --- |
| `run [args]` (`r`) | Start from the beginning; run until a breakpoint, crash, or exit |
| `start [args]` | Start from the beginning; stop on the first line of `main` |
| `continue` (`c`) | Resume after a stop |
| Ctrl+C | Pause the running program (doesn't kill it) |
| `finish` | Run until the selected function returns; prints the return value |
| `until <line>` | Run until that line is reached (handy for leaving a loop) |
| `kill` | Stop the program but stay in gdb |

`[Inferior 1 (process …) exited normally]` means the program finished with code 0. After that, `No stack` and `No frame selected` just mean nothing is running.

## Breakpoints

A breakpoint only marks a spot; you still need `run` or `continue` to reach it.

| Command | What it does |
| --- | --- |
| `break <function>` (`b`) | Stop when the function is entered, e.g. `b similar_files_checker::findSameFullHashFiles` |
| `b file.cpp:42` | Stop at a line in a file |
| `b 42` | Stop at a line in the current file |
| `b file.cpp:42 if shared_counter == 0` | Stop only when the condition is true |
| `tbreak <location>` | One-time breakpoint, deleted after its first hit |
| `watch <expr>` | Stop whenever that value changes |
| `info breakpoints` (`i b`) | List breakpoints with numbers and hit counts |
| `disable [N]` / `enable [N]` | Turn one (or all) off or on without deleting |
| `delete [N]` | Remove one (or all) |

In `info breakpoints`, a healthy entry shows `at file.cpp:line`. A bare function name with no file means the binary has no debug info.

## Stepping

The line gdb shows is the next one to run; it hasn't executed yet.

| Command | What it does |
| --- | --- |
| `next` (`n`) | Run the current line, stepping over any calls in it |
| `step` (`s`) | Run the current line, stepping into the first call |
| `finish` | Run until the current function returns (step out) |
| `until` | At the end of a loop, run until the loop exits |
| `next 5` | Repeat `next` five times |
| Enter | Repeat the last command |

A line that calls into a long pipeline (like your `findSameSizeFiles(...).and_then(...)` chain) runs the whole thing under `next`. Set a breakpoint inside instead.

## Inspecting variables

Everything here reads from the selected frame, so check `frame` if a name isn't found.

| Command | What it does |
| --- | --- |
| `print <expr>` (`p`) | Show a value: `p files.size()`, `p *ptr`, `p vec[3]`, `p a + b` |
| `p/x <expr>` | Print in hex (`/d` decimal, `/t` binary) |
| `info locals` | Every local variable in the current function |
| `info args` | The current function's parameters |
| `display <expr>` | Re-print this after every stop; `undisplay N` stops it |
| `ptype <expr>` | Show the type of an expression |
| `set print elements 10` | Cap how many container items get printed (`0` = no limit) |
| `set var x = 5` | Change a variable while paused |

If `p v.size()` fails with `may be inlined`, use `p v` and read the length from the header line.

## Call stack and frames

`bt` lists calls from where execution is (`#0`) out to `main`; the `#N` numbers are what `frame N` takes.

| Command | What it does |
| --- | --- |
| `bt` (`where`) | Show the call stack of the current thread |
| `bt full` | Same, plus every frame's local variables |
| `frame` (`f`) | Show the selected frame: function, file, line |
| `frame N` | Select frame `#N` from `bt` |
| `up` / `down` | Move one frame toward `main` / back toward `#0` |
| `frame 0` | Return to where execution actually is |
| `list` (`l`) | Show source around the selected line; repeat to scroll |
| `list <function>` / `list 40,60` | Show a function's source, or a line range |
| `search <regex>` | Find the next source line matching the pattern |

## Threads

Each thread has its own stack; `bt`, `frame` and `print` only describe the selected thread.

| Command | What it does |
| --- | --- |
| `info threads` | List all threads; `*` marks the selected one |
| `thread` | Show which thread is selected |
| `thread N` | Switch to thread N (thread 1 is the one running `main`) |
| `thread apply all bt` | Call stack of every thread at once, the go-to for hangs |
| `thread apply all bt full` | Same, with locals |
| `set scheduler-locking step` | While stepping, freeze the other threads; `off` to undo |

- Worker stacks bottom out in `start_thread`; the main thread's ends in `main`.
- Thread numbers are never reused, so a high number doesn't mean many live threads.

## Views and settings

`layout src` is the fastest way to always see where you are.

| Command | What it does |
| --- | --- |
| `layout src` | Split screen: source on top with the current line highlighted |
| `layout split` | Source and assembly together |
| `tui disable` or Ctrl+X then A | Leave split-screen mode |
| Ctrl+L | Redraw a garbled screen |
| `set pagination off` | Stop the "press Enter" pauses on long output |
| `set print pretty on` | Print structs one field per line |
| `shell <cmd>` | Run a shell command without leaving gdb |

Put the settings you always want in `~/.gdbinit` so they apply every session:

```
set pagination off
set print pretty on
set debuginfod enabled off
```

## Recipes

**Program seems hung**

1. `run`, then Ctrl+C when it stalls.
2. `thread apply all bt` and note where each thread sits.
3. `continue`, wait 10–20 seconds, Ctrl+C, `thread apply all bt` again.
4. Same stacks with the same data means stuck: all threads in `cv.wait` points to a termination bug; frames in `read`, `openat` or `getdents` point to a stalled disk or network mount. Changing stacks or files means it's just slow.

**Program crashes (segfault)**

1. `run`; gdb stops on the signal by itself.
2. `bt` to see the call chain.
3. `frame N` on the first frame in your own code, then `info locals` and `print` the suspect pointer or index.

**A test's assert fails**

1. Use a `Debug` build so asserts exist, and run the test binary directly, not through `ctest`.
2. `run`; the failed assert raises `SIGABRT` and gdb stops.
3. `bt`, find the frame in `test_main.cpp`, `frame N`, and inspect the values it checked.

**Something changes a value and you don't know what**

1. Break where the variable exists, then `watch <variable>`.
2. `continue`; gdb stops at each line that modifies it and shows old and new values.

## Gotchas

Most confusing gdb moments trace back to one of these.

| Symptom | Likely cause |
| --- | --- |
| Breakpoint shows no `file:line`; `No symbol table info available` | No debug info, or the wrong binary: check the `Starting program:` path |
| `No symbol "x" in current context` | Wrong frame or thread selected, or no debug info |
| A wildly wrong value (a size in the trillions) | That variable's line hasn't run yet, so it's uninitialized stack memory |
| `No frame selected` / `No stack` | The program isn't running (not started, or already exited) |
| `finish` only ends one worker thread | `finish` acts on the selected frame: `thread 1`, `bt`, `frame N` first |
| Output appears only at the end | stdout is buffered when piped (ctest); run the binary directly or flush |
| A failing assert passes | `NDEBUG` is defined (Release or RelWithDebInfo build) |
