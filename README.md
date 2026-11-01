# cash — C Shell

A Unix shell written in C.

## Demo

_GIF coming soon_

## Features

- Execute external commands via `fork` + `execvp`
- Builtins: `cd`, `exit`
- Colored prompt displaying current working directory

## Build

```sh
make        # build
make clean  # remove binary
```

Requires a C17-compatible compiler (gcc or clang).

## Usage

```sh
./cash
```

```
/Users/luca/projects
$ ls
README.md  Makefile  src/
$ cd src
/Users/luca/projects/src
$ exit
```

## Architecture

| File | Responsibility |
|------|----------------|
| `src/main.c` | REPL loop, line parsing, process execution |
| `src/builtins.c` | Built-in command implementations |
| `src/builtins.h` | Builtin type definitions and dispatch table |

## Roadmap

See [Issues](../../issues).
