#pragma once
#include <cstdint>
#include "row.h"
#include "pager.h"
#include "cursor.h"

enum class NodeType : uint8_t { INTERNAL, LEAF };

const uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);
const uint32_t NODE_TYPE_OFFSET = 0;
const uint32_t IS_ROOT_SIZE = sizeof(uint8_t);
const uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;
const uint32_t PARENT_POINTER_SIZE = sizeof(uint32_t);
const uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;
const uint32_t COMMON_NODE_HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

const uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE;

const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_KEY_OFFSET = 0;
const uint32_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;
const uint32_t LEAF_NODE_VALUE_OFFSET = LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
const uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
const uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_MAX_CELLS = LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;

// Simplified accessors
#define LEAF_NODE_NUM_CELLS(node) (*((uint32_t*)((char*)(node) + LEAF_NODE_NUM_CELLS_OFFSET)))
#define LEAF_NODE_CELL(node, cell_num) ((char*)(node) + LEAF_NODE_HEADER_SIZE + (cell_num) * LEAF_NODE_CELL_SIZE)
#define LEAF_NODE_KEY(node, cell_num) (*((uint32_t*)LEAF_NODE_CELL(node, cell_num)))
#define LEAF_NODE_VALUE(node, cell_num) (LEAF_NODE_CELL(node, cell_num) + LEAF_NODE_KEY_SIZE)

#define GET_NODE_TYPE(node) ((NodeType)*((uint8_t*)((char*)(node) + NODE_TYPE_OFFSET)))
#define SET_NODE_TYPE(node, type) (*((uint8_t*)((char*)(node) + NODE_TYPE_OFFSET)) = (uint8_t)(type))

#define IS_ROOT(node) (*((uint8_t*)((char*)(node) + IS_ROOT_OFFSET)))
#define SET_ROOT(node, is_root) (*((uint8_t*)((char*)(node) + IS_ROOT_OFFSET)) = (is_root) ? 1 : 0)

void initialize_leaf_node(void* node);
void leaf_node_insert(Cursor& cursor, uint32_t key, Row& value);
void print_constants();
void print_leaf_node(void* node);