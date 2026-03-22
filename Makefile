CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
SRC_DIR  = src
INC_DIR  = include
TARGET   = db

SOURCES  = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS  = $(SOURCES:.cpp=.o)

.PHONY: all run clean test

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $^ -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) test.db src/test.db

test: $(TARGET)
	@echo "=== Running tests ==="
	@rm -f test.db 2>/dev/null; \
	echo "insert 1 user1 email1" | ./$(TARGET) | grep -q "Executed" && echo "✓ Insert works" || echo "✗ Insert failed"
	@rm -f test.db 2>/dev/null; \
	echo -e "insert 1 a b\ninsert 2 c d\nselect\n.exit" | ./$(TARGET) | grep -q "1 a b" && echo "✓ Select works" || echo "✗ Select failed"
	@rm -f test.db 2>/dev/null; \
	echo ".constants" | ./$(TARGET) | grep -q "ROW_SIZE" && echo "✓ Constants work" || echo "✗ Constants failed"
	@rm -f test.db 2>/dev/null; \
	echo -e "insert 1 x y\n.btree\n.exit" | ./$(TARGET) | grep -q "leaf" && echo "✓ B-tree works" || echo "✗ B-tree failed"
	@echo "=== Tests complete ==="
