# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Hydra is a terminal UI tool inspired by the Emacs Hydra package. It reads CSV configuration files
defining command hierarchies and presents an interactive TUI for navigating and executing shell
commands. The selected command is printed to stdout for `eval` in a shell script.

## Build & Test Commands

```bash
make          # Build the hydra binary
make test     # Compile the test suite
make run-test # Compile and run tests
make run      # Build and run with the bundled git hydra file
make install  # Install to $(DESTDIR)$(prefix)/bin (prefix defaults to /usr)
make clean    # Remove build artifacts (*.o, hydra, test binaries)
```

Compiler flags: `-O3 -Wextra -Wall`. No external dependencies — pure C with POSIX/libc only.

## Architecture

The project is ~300 lines of C split into three files:

- **`hydra.h`** — type definitions, ANSI color constants, and function prototypes
- **`hydra.c`** — all logic: CSV parser, command tree manipulation, TUI rendering, terminal I/O
- **`main.c`** — thin entry point: parse CLI args, load files, call `Start()`
- **`test.c`** — unit tests using `assert.h`

### Core Data Structure

Commands form a tree via linked lists:

```c
typedef struct Command {
  char key;               // single-char trigger key
  char* name;             // display label
  char* command;          // shell command (NULL for branch nodes)
  struct Command* children; // first child
  struct Command* next;     // next sibling (sorted by key)
} Command;
```

### CSV Config Format

Each hydra file uses three columns: `key_path,name,command`

- `key_path` is a string of characters forming the navigation path (e.g., `"gbc"` = g→b→c)
- `command` is empty for intermediate branch nodes, non-empty for leaf nodes
- Multiple files can be loaded; later definitions override earlier ones

### TUI Rendering

- Output goes to **stderr** (the TUI display); the selected command goes to **stdout**
- Uses raw terminal mode (`termios`) for single-char input without buffering
- Terminal width is detected via `ioctl(TIOCGWINSZ)` for responsive multi-column layout
- `ClearLines()` erases the TUI before printing the final command

### Shell Integration

`hydra-completion.bash` binds the Space key to `eval $(hydra ...)`. The `$HYDRA` environment
variable can be set to a space-separated list of hydra files to load.

## Example Hydra Files

The `hydras/` directory contains bundled examples: `git`, `npm`, `systemd`, `cargo`, `pacman`,
`vi`, `yarn`, `make`. These are installed to `/usr/share/hydra/hydras/`.
