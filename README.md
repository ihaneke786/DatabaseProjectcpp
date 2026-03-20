# C++ Database Implementation

A simple B-Tree based database implementation following the SQLite tutorial.

## Building

Compile all source files:
```bash
g++ -std=c++17 -Iinclude src/*.cpp -o db
```

## Running

Start the database REPL:
```bash
./db
```

## Commands

### SQL Commands
- `insert <id> <username> <email>` - Insert a new row
- `select` - Display all rows

### Meta Commands
- `.exit` - Exit the database
- `.constants` - Show database constants
- `.btree` - Show B-Tree structure

## Testing

### Basic Test
```bash
# Clean start
rm -f test.db

# Insert and select
printf "insert 1 user1 email1\ninsert 2 user2 email2\nselect\n.exit\n" | ./db
```

Expected output:
```
db > Executed.
db > Executed.
db > 1 user1 email1
2 user2 email2
Executed.
```

### B-Tree Structure Test
```bash
printf "insert 1 user1 email1\ninsert 2 user2 email2\n.btree\n.exit\n" | ./db
```

Expected output:
```
db > Executed.
db > Executed.
db > Leaf (size 2)
  - 0 : 1
  - 1 : 2
```

### Constants Test
```bash
printf ".constants\n.exit\n" | ./db
```

Expected output:
```
db > ROW_SIZE: 68
COMMON_NODE_HEADER_SIZE: 6
LEAF_NODE_HEADER_SIZE: 10
LEAF_NODE_CELL_SIZE: 72
LEAF_NODE_SPACE_FOR_CELLS: 4086
LEAF_NODE_MAX_CELLS: 56
```

### Interactive Mode
```bash
./db
db > insert 1 alice alice@example.com
Executed.
db > insert 2 bob bob@example.com
Executed.
db > select
1 alice alice@example.com
2 bob bob@example.com
Executed.
db > .btree
Leaf (size 2)
  - 0 : 1
  - 1 : 2
db > .exit
```

## Architecture

- **B-Tree**: Uses leaf nodes for data storage
- **Pager**: Manages page caching and disk I/O
- **Cursor**: Handles table traversal
- **REPL**: Read-Eval-Print Loop for user interaction

## Files

- `include/*.h` - Header files
- `src/*.cpp` - Implementation files
- `db` - Compiled executable
