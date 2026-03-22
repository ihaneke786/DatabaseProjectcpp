# C++ Database Implementation

A B-Tree based embedded database written in C++17, inspired by [cstack's SQLite tutorial](https://cstack.github.io/db_tutorial/). Stores rows on disk with page caching, supports tree splits at leaf and internal nodes, and exposes a REPL for interactive use.

## Quick Start

```bash
make              # build
make run          # start REPL
make test         # run tests
make clean        # remove db binary and test.db
```

## Usage

| Command | Example |
|---------|---------|
| `insert <id> <username> <email>` | `insert 1 alice alice@example.com` |
| `select` | Display all rows |
| `.exit` | Quit |
| `.btree` | Print B-Tree structure |
| `.constants` | Show row/node sizes |

Data persists in `test.db` in the current directory.

## Architecture

- **B-Tree** — Leaf nodes store rows; internal nodes index keys; supports splitting at both levels
- **Pager** — 4KB page cache with disk I/O
- **Cursor** — Table traversal (find, advance, start)
- **REPL** — Command parsing and execution loop

## Project Structure

```
include/     headers (btree, cursor, pager, row, table, statement, repl)
src/         implementation
```
