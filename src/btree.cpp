#include "btree.h"
#include "cursor.h"
#include "table.h"
#include "row.h"
#include <iostream>
#include <cstring>

void initialize_leaf_node(void* node) {
    SET_NODE_TYPE(node, NodeType::LEAF);
    SET_ROOT(node, false);
    LEAF_NODE_NUM_CELLS(node) = 0;
}

void leaf_node_insert(Cursor& cursor, uint32_t key, Row& value) {
    void* node = cursor.table->pager.get_page(cursor.page_num);
    uint32_t num_cells = LEAF_NODE_NUM_CELLS(node);

    if (num_cells >= LEAF_NODE_MAX_CELLS) {
        std::cerr << "Need to implement splitting a leaf node.\n";
        exit(EXIT_FAILURE);
    }

    if (cursor.cell_num < num_cells) {
        for (uint32_t i = num_cells; i > cursor.cell_num; i--) {
            memcpy(LEAF_NODE_CELL(node, i), LEAF_NODE_CELL(node, i - 1), LEAF_NODE_CELL_SIZE);
        }
    }

    LEAF_NODE_NUM_CELLS(node)++;
    LEAF_NODE_KEY(node, cursor.cell_num) = key;
    serialize_row(&value, LEAF_NODE_VALUE(node, cursor.cell_num));
}


// print info to test
void print_constants() {
    std::cout << "ROW_SIZE: " << ROW_SIZE << "\n";
    std::cout << "COMMON_NODE_HEADER_SIZE: " << COMMON_NODE_HEADER_SIZE << "\n";
    std::cout << "LEAF_NODE_HEADER_SIZE: " << LEAF_NODE_HEADER_SIZE << "\n";
    std::cout << "LEAF_NODE_CELL_SIZE: " << LEAF_NODE_CELL_SIZE << "\n";
    std::cout << "LEAF_NODE_SPACE_FOR_CELLS: " << LEAF_NODE_SPACE_FOR_CELLS << "\n";
    std::cout << "LEAF_NODE_MAX_CELLS: " << LEAF_NODE_MAX_CELLS << "\n";
}


void print_leaf_node(void* node) {
    uint32_t num_cells = LEAF_NODE_NUM_CELLS(node);
    std::cout << "Leaf (size " << num_cells << ")\n";
    for (uint32_t i = 0; i < num_cells; i++) {
        uint32_t key = LEAF_NODE_KEY(node, i);
        std::cout << "  - " << i << " : " << key << "\n";
    }
}