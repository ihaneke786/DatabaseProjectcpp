#pragma once
#include <cstdint>
#include "row.h"
#include "pager.h"
#include "cursor.h"

enum class NodeType : uint8_t {
    INTERNAL = 0,
    LEAF = 1
};

const uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);
const uint32_t NODE_TYPE_OFFSET = 0;
const uint32_t IS_ROOT_SIZE = sizeof(uint8_t);
const uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;
const uint32_t PARENT_POINTER_SIZE = sizeof(uint32_t);
const uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;
const uint32_t COMMON_NODE_HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

const uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_NEXT_LEAF_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_NEXT_LEAF_OFFSET =
    LEAF_NODE_NUM_CELLS_OFFSET + LEAF_NODE_NUM_CELLS_SIZE;
const uint32_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE +
                                       LEAF_NODE_NUM_CELLS_SIZE +
                                       LEAF_NODE_NEXT_LEAF_SIZE;

const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_KEY_OFFSET = 0;
const uint32_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;
const uint32_t LEAF_NODE_VALUE_OFFSET = LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
const uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
const uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_MAX_CELLS = LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;
const uint32_t LEAF_NODE_RIGHT_SPLIT_COUNT = (LEAF_NODE_MAX_CELLS + 1) / 2;
const uint32_t LEAF_NODE_LEFT_SPLIT_COUNT = (LEAF_NODE_MAX_CELLS + 1) - LEAF_NODE_RIGHT_SPLIT_COUNT;

/*
 * Internal Node Header Layout
 */
const uint32_t INTERNAL_NODE_NUM_KEYS_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_NUM_KEYS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t INTERNAL_NODE_RIGHT_CHILD_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_RIGHT_CHILD_OFFSET =
    INTERNAL_NODE_NUM_KEYS_OFFSET + INTERNAL_NODE_NUM_KEYS_SIZE;
const uint32_t INTERNAL_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE +
                                           INTERNAL_NODE_NUM_KEYS_SIZE +
                                           INTERNAL_NODE_RIGHT_CHILD_SIZE;

/*
 * Internal Node Body Layout
 */
const uint32_t INTERNAL_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_CHILD_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_CELL_SIZE =
    INTERNAL_NODE_CHILD_SIZE + INTERNAL_NODE_KEY_SIZE;
/* Keep this small for testing */
const uint32_t INTERNAL_NODE_MAX_CELLS = 3;

const uint32_t INVALID_PAGE_NUM = UINT32_MAX;

// Simplified accessors
#define LEAF_NODE_NUM_CELLS(node) (*((uint32_t*)((char*)(node) + LEAF_NODE_NUM_CELLS_OFFSET)))
#define LEAF_NODE_CELL(node, cell_num) ((char*)(node) + LEAF_NODE_HEADER_SIZE + (cell_num) * LEAF_NODE_CELL_SIZE)
#define LEAF_NODE_KEY(node, cell_num) (*((uint32_t*)LEAF_NODE_CELL(node, cell_num)))
#define LEAF_NODE_VALUE(node, cell_num) (LEAF_NODE_CELL(node, cell_num) + LEAF_NODE_KEY_SIZE)

#define GET_NODE_TYPE(node) ((NodeType)*((uint8_t*)((char*)(node) + NODE_TYPE_OFFSET)))
#define SET_NODE_TYPE(node, type) (*((uint8_t*)((char*)(node) + NODE_TYPE_OFFSET)) = (uint8_t)(type))

#define IS_ROOT(node) (*((uint8_t*)((char*)(node) + IS_ROOT_OFFSET)))
#define SET_ROOT(node, is_root) (*((uint8_t*)((char*)(node) + IS_ROOT_OFFSET)) = (is_root) ? 1 : 0)

// Leaf node accessors (matching tutorial function names)
uint32_t* leaf_node_num_cells(void* node);
void* leaf_node_cell(void* node, uint32_t cell_num);
uint32_t* leaf_node_key(void* node, uint32_t cell_num);
void* leaf_node_value(void* node, uint32_t cell_num);
uint32_t* leaf_node_next_leaf(void* node);

// Internal node accessors
uint32_t* internal_node_num_keys(void* node);
uint32_t* internal_node_right_child(void* node);
uint32_t* internal_node_cell(void* node, uint32_t cell_num);
uint32_t* internal_node_child(void* node, uint32_t child_num);
uint32_t* internal_node_key(void* node, uint32_t key_num);

uint32_t* node_parent(void* node);

NodeType get_node_type(void* node);
void set_node_type(void* node, NodeType type);
bool is_node_root(void* node);
void set_node_root(void* node, bool is_root);
uint32_t get_node_max_key(Pager& pager, void* node);

void initialize_leaf_node(void* node);
void initialize_internal_node(void* node);
void leaf_node_insert(Cursor& cursor, uint32_t key, Row& value);
void leaf_node_split_and_insert(Cursor& cursor, uint32_t key, Row& value);
void create_new_root(Table& table, uint32_t right_child_page_num);
uint32_t internal_node_find_child(void* node, uint32_t key);
void update_internal_node_key(void* node, uint32_t old_key, uint32_t new_key);
void internal_node_split_and_insert(Table& table, uint32_t parent_page_num,
                                   uint32_t child_page_num);
void internal_node_insert(Table& table, uint32_t parent_page_num,
                         uint32_t child_page_num);

void print_constants();
void indent(uint32_t level);
void print_tree(Pager& pager, uint32_t page_num, uint32_t indentation_level);

Cursor internal_node_find(Table& table, uint32_t page_num, uint32_t key);
Cursor leaf_node_find(Table& table, uint32_t page_num, uint32_t key);
