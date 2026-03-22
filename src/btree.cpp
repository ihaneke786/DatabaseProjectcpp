#include "btree.h"
#include "cursor.h"
#include "table.h"
#include "row.h"
#include "pager.h"
#include <iostream>
#include <cstring>
#include <cstdio>

// Leaf node accessors 
uint32_t* leaf_node_num_cells(void* node) {
    return reinterpret_cast<uint32_t*>(static_cast<char*>(node) + LEAF_NODE_NUM_CELLS_OFFSET);
}

void* leaf_node_cell(void* node, uint32_t cell_num) {
    return static_cast<char*>(node) + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

uint32_t* leaf_node_key(void* node, uint32_t cell_num) {
    return reinterpret_cast<uint32_t*>(static_cast<char*>(leaf_node_cell(node, cell_num)));
}

void* leaf_node_value(void* node, uint32_t cell_num) {
    return static_cast<char*>(leaf_node_cell(node, cell_num)) + LEAF_NODE_KEY_SIZE;
}

uint32_t* leaf_node_next_leaf(void* node) {
    return reinterpret_cast<uint32_t*>(static_cast<char*>(node) + LEAF_NODE_NEXT_LEAF_OFFSET);
}

// Internal node accessors
uint32_t* internal_node_num_keys(void* node) {
    return reinterpret_cast<uint32_t*>(static_cast<char*>(node) + INTERNAL_NODE_NUM_KEYS_OFFSET);
}

uint32_t* internal_node_right_child(void* node) {
    return reinterpret_cast<uint32_t*>(static_cast<char*>(node) + INTERNAL_NODE_RIGHT_CHILD_OFFSET);
}

uint32_t* internal_node_cell(void* node, uint32_t cell_num) {
    return reinterpret_cast<uint32_t*>(static_cast<char*>(node) + INTERNAL_NODE_HEADER_SIZE +
                                       cell_num * INTERNAL_NODE_CELL_SIZE);
}

uint32_t* internal_node_child(void* node, uint32_t child_num) {
    uint32_t num_keys = *internal_node_num_keys(node);
    if (child_num > num_keys) {
        printf("Tried to access child_num %d > num_keys %d\n", (int)child_num, (int)num_keys);
        exit(EXIT_FAILURE);
    } else if (child_num == num_keys) {
        uint32_t* right_child = internal_node_right_child(node);
        if (*right_child == INVALID_PAGE_NUM) {
            printf("Tried to access right child of node, but was invalid page\n");
            exit(EXIT_FAILURE);
        }
        return right_child;
    } else {
        uint32_t* child_ptr = internal_node_cell(node, child_num);
        if (*child_ptr == INVALID_PAGE_NUM) {
            printf("Tried to access child %d of node, but was invalid page\n", (int)child_num);
            exit(EXIT_FAILURE);
        }
        return child_ptr;
    }
}

uint32_t* internal_node_key(void* node, uint32_t key_num) {
    return reinterpret_cast<uint32_t*>(
        static_cast<char*>(reinterpret_cast<void*>(internal_node_cell(node, key_num))) +
        INTERNAL_NODE_CHILD_SIZE);
}

uint32_t* node_parent(void* node) {
    return reinterpret_cast<uint32_t*>(static_cast<char*>(node) + PARENT_POINTER_OFFSET);
}

NodeType get_node_type(void* node) {
    uint8_t value = *reinterpret_cast<uint8_t*>(static_cast<char*>(node) + NODE_TYPE_OFFSET);
    return static_cast<NodeType>(value);
}

void set_node_type(void* node, NodeType type) {
    uint8_t value = static_cast<uint8_t>(type);
    *reinterpret_cast<uint8_t*>(static_cast<char*>(node) + NODE_TYPE_OFFSET) = value;
}

bool is_node_root(void* node) {
    uint8_t value = *reinterpret_cast<uint8_t*>(static_cast<char*>(node) + IS_ROOT_OFFSET);
    return static_cast<bool>(value);
}

void set_node_root(void* node, bool is_root) {
    uint8_t value = is_root ? 1 : 0;
    *reinterpret_cast<uint8_t*>(static_cast<char*>(node) + IS_ROOT_OFFSET) = value;
}

uint32_t get_node_max_key(Pager& pager, void* node) {
    if (get_node_type(node) == NodeType::LEAF) {
        return *leaf_node_key(node, *leaf_node_num_cells(node) - 1);
    }
    void* right_child = pager.get_page(*internal_node_right_child(node));
    return get_node_max_key(pager, right_child);
}

void initialize_leaf_node(void* node) {
    set_node_type(node, NodeType::LEAF);
    set_node_root(node, false);
    *leaf_node_num_cells(node) = 0;
    *leaf_node_next_leaf(node) = 0;  // 0 represents no sibling
}

void initialize_internal_node(void* node) {
    set_node_type(node, NodeType::INTERNAL);
    set_node_root(node, false);
    *internal_node_num_keys(node) = 0;
    /*
     * Necessary because the root page number is 0; by not initializing an internal
     * node's right child to an invalid page number when initializing the node, we may
     * end up with 0 as the node's right child, which makes the node a parent of the root
     */
    *internal_node_right_child(node) = INVALID_PAGE_NUM;
}

void leaf_node_split_and_insert(Cursor& cursor, uint32_t key, Row& value) {
    /*
     * Create a new node and move half the cells over.
     * Insert the new value in one of the two nodes.
     * Update parent or create a new parent.
     */

    void* old_node = cursor.table->pager.get_page(cursor.page_num);
    uint32_t old_max = get_node_max_key(cursor.table->pager, old_node);
    uint32_t new_page_num = cursor.table->pager.get_unused_page_num();
    void* new_node = cursor.table->pager.get_page(new_page_num);
    initialize_leaf_node(new_node);
    *node_parent(new_node) = *node_parent(old_node);
    *leaf_node_next_leaf(new_node) = *leaf_node_next_leaf(old_node);
    *leaf_node_next_leaf(old_node) = new_page_num;

    /*
     * All existing keys plus new key should be divided
     * evenly between old (left) and new (right) nodes.
     * Starting from the right, move each key to correct position.
     */
    for (int32_t i = static_cast<int32_t>(LEAF_NODE_MAX_CELLS); i >= 0; i--) {
        void* destination_node;
        if (i >= static_cast<int32_t>(LEAF_NODE_LEFT_SPLIT_COUNT)) {
            destination_node = new_node;
        } else {
            destination_node = old_node;
        }
        uint32_t index_within_node = static_cast<uint32_t>(i) % LEAF_NODE_LEFT_SPLIT_COUNT;
        void* destination = leaf_node_cell(destination_node, index_within_node);

        if (i == static_cast<int32_t>(cursor.cell_num)) {
            serialize_row(&value, leaf_node_value(destination_node, index_within_node));
            *leaf_node_key(destination_node, index_within_node) = key;
        } else if (i > static_cast<int32_t>(cursor.cell_num)) {
            memcpy(destination, leaf_node_cell(old_node, static_cast<uint32_t>(i - 1)),
                   LEAF_NODE_CELL_SIZE);
        } else {
            memcpy(destination, leaf_node_cell(old_node, static_cast<uint32_t>(i)),
                   LEAF_NODE_CELL_SIZE);
        }
    }

    /* Update cell count on both leaf nodes */
    *leaf_node_num_cells(old_node) = LEAF_NODE_LEFT_SPLIT_COUNT;
    *leaf_node_num_cells(new_node) = LEAF_NODE_RIGHT_SPLIT_COUNT;

    if (is_node_root(old_node)) {
        create_new_root(*cursor.table, new_page_num);
    } else {
        uint32_t parent_page_num = *node_parent(old_node);
        uint32_t new_max = get_node_max_key(cursor.table->pager, old_node);
        void* parent = cursor.table->pager.get_page(parent_page_num);

        update_internal_node_key(parent, old_max, new_max);
        internal_node_insert(*cursor.table, parent_page_num, new_page_num);
    }
}

void create_new_root(Table& table, uint32_t right_child_page_num) {
    /*
     * Handle splitting the root.
     * Old root copied to new page, becomes left child.
     * Address of right child passed in.
     * Re-initialize root page to contain the new root node.
     * New root node points to two children.
     */

    void* root = table.pager.get_page(table.root_page_num);
    void* right_child = table.pager.get_page(right_child_page_num);
    uint32_t left_child_page_num = table.pager.get_unused_page_num();
    void* left_child = table.pager.get_page(left_child_page_num);

    if (get_node_type(root) == NodeType::INTERNAL) {
        initialize_internal_node(right_child);
        initialize_internal_node(left_child);
    }

    /* Left child has data copied from old root */
    memcpy(left_child, root, PAGE_SIZE);
    set_node_root(left_child, false);

    if (get_node_type(left_child) == NodeType::INTERNAL) {
        for (uint32_t i = 0; i < *internal_node_num_keys(left_child); i++) {
            void* child = table.pager.get_page(*internal_node_child(left_child, i));
            *node_parent(child) = left_child_page_num;
        }
        void* child = table.pager.get_page(*internal_node_right_child(left_child));
        *node_parent(child) = left_child_page_num;
    } else {
        *leaf_node_next_leaf(left_child) = right_child_page_num;
    }

    /* Root node is a new internal node with one key and two children */
    initialize_internal_node(root);
    set_node_root(root, true);
    *internal_node_num_keys(root) = 1;
    *internal_node_child(root, 0) = left_child_page_num;
    uint32_t left_child_max_key = get_node_max_key(table.pager, left_child);
    *internal_node_key(root, 0) = left_child_max_key;
    *internal_node_right_child(root) = right_child_page_num;
    *node_parent(left_child) = table.root_page_num;
    *node_parent(right_child) = table.root_page_num;
}

void leaf_node_insert(Cursor& cursor, uint32_t key, Row& value) {
    void* node = cursor.table->pager.get_page(cursor.page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);

    if (num_cells >= LEAF_NODE_MAX_CELLS) {
        leaf_node_split_and_insert(cursor, key, value);
        return;
    }

    if (cursor.cell_num < num_cells) {
        for (uint32_t i = num_cells; i > cursor.cell_num; i--) {
            memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i - 1), LEAF_NODE_CELL_SIZE);
        }
    }

    *leaf_node_num_cells(node) += 1;
    *leaf_node_key(node, cursor.cell_num) = key;
    serialize_row(&value, static_cast<char*>(leaf_node_cell(node, cursor.cell_num)) + LEAF_NODE_KEY_SIZE);
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

void indent(uint32_t level) {
    for (uint32_t i = 0; i < level; i++) {
        printf("  ");
    }
}

void print_tree(Pager& pager, uint32_t page_num, uint32_t indentation_level) {
    void* node = pager.get_page(page_num);
    uint32_t num_keys;
    uint32_t child;

    switch (get_node_type(node)) {
        case NodeType::LEAF:
            num_keys = *leaf_node_num_cells(node);
            indent(indentation_level);
            printf("- leaf (size %d)\n", (int)num_keys);
            for (uint32_t i = 0; i < num_keys; i++) {
                indent(indentation_level + 1);
                printf("- %d\n", (int)*leaf_node_key(node, i));
            }
            break;
        case NodeType::INTERNAL:
            num_keys = *internal_node_num_keys(node);
            indent(indentation_level);
            printf("- internal (size %d)\n", (int)num_keys);
            if (num_keys > 0) {
                for (uint32_t i = 0; i < num_keys; i++) {
                    child = *internal_node_child(node, i);
                    print_tree(pager, child, indentation_level + 1);

                    indent(indentation_level + 1);
                    printf("- key %d\n", (int)*internal_node_key(node, i));
                }
                child = *internal_node_right_child(node);
                print_tree(pager, child, indentation_level + 1);
            }
            break;
    }
}

// Binary search for leaf node
Cursor leaf_node_find(Table& table, uint32_t page_num, uint32_t key) {
    void* node = table.pager.get_page(page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);

    Cursor cursor;
    cursor.table = &table;
    cursor.page_num = page_num;

    // Binary search
    uint32_t min_index = 0;
    uint32_t max_index = num_cells;
    while (min_index != max_index) {
        uint32_t index = (min_index + max_index) / 2;
        uint32_t key_at_index = *leaf_node_key(node, index);
        if (key == key_at_index) {
            cursor.cell_num = index;
            cursor.end_of_table = false;
            return cursor;
        }
        if (key < key_at_index) {
            max_index = index;
        } else {
            min_index = index + 1;
        }
    }

    cursor.cell_num = min_index;
    cursor.end_of_table = (min_index == num_cells);

    return cursor;
}

uint32_t internal_node_find_child(void* node, uint32_t key) {
    /*
     * Return the index of the child which should contain
     * the given key.
     */
    uint32_t num_keys = *internal_node_num_keys(node);

    /* Binary search */
    uint32_t min_index = 0;
    uint32_t max_index = num_keys; /* there is one more child than key */

    while (min_index != max_index) {
        uint32_t index = (min_index + max_index) / 2;
        uint32_t key_to_right = *internal_node_key(node, index);
        if (key_to_right >= key) {
            max_index = index;
        } else {
            min_index = index + 1;
        }
    }

    return min_index;
}

void update_internal_node_key(void* node, uint32_t old_key, uint32_t new_key) {
    uint32_t old_child_index = internal_node_find_child(node, old_key);
    *internal_node_key(node, old_child_index) = new_key;
}

void internal_node_split_and_insert(Table& table, uint32_t parent_page_num,
                                   uint32_t child_page_num);

void internal_node_insert(Table& table, uint32_t parent_page_num,
                         uint32_t child_page_num) {
    /*
     * Add a new child/key pair to parent that corresponds to child
     */
    void* parent = table.pager.get_page(parent_page_num);
    void* child = table.pager.get_page(child_page_num);
    uint32_t child_max_key = get_node_max_key(table.pager, child);
    uint32_t index = internal_node_find_child(parent, child_max_key);

    uint32_t original_num_keys = *internal_node_num_keys(parent);

    if (original_num_keys >= INTERNAL_NODE_MAX_CELLS) {
        internal_node_split_and_insert(table, parent_page_num, child_page_num);
        return;
    }

    uint32_t right_child_page_num = *internal_node_right_child(parent);
    /*
     * An internal node with a right child of INVALID_PAGE_NUM is empty
     */
    if (right_child_page_num == INVALID_PAGE_NUM) {
        *internal_node_right_child(parent) = child_page_num;
        return;
    }

    void* right_child = table.pager.get_page(right_child_page_num);
    /*
     * If we are already at the max number of cells for a node, we cannot increment
     * before splitting. Incrementing without inserting a new key/child pair
     * and immediately calling internal_node_split_and_insert has the effect
     * of creating a new key at (max_cells + 1) with an uninitialized value
     */
    *internal_node_num_keys(parent) = original_num_keys + 1;

    if (child_max_key > get_node_max_key(table.pager, right_child)) {
        /* Replace right child */
        *internal_node_child(parent, original_num_keys) = right_child_page_num;
        *internal_node_key(parent, original_num_keys) =
            get_node_max_key(table.pager, right_child);
        *internal_node_right_child(parent) = child_page_num;
    } else {
        /* Make room for the new cell */
        for (uint32_t i = original_num_keys; i > index; i--) {
            void* destination = internal_node_cell(parent, i);
            void* source = internal_node_cell(parent, i - 1);
            memcpy(destination, source, INTERNAL_NODE_CELL_SIZE);
        }
        *internal_node_child(parent, index) = child_page_num;
        *internal_node_key(parent, index) = child_max_key;
    }
}

void internal_node_split_and_insert(Table& table, uint32_t parent_page_num,
                                   uint32_t child_page_num) {
    uint32_t old_page_num = parent_page_num;
    void* old_node = table.pager.get_page(parent_page_num);
    uint32_t old_max = get_node_max_key(table.pager, old_node);

    void* child = table.pager.get_page(child_page_num);
    uint32_t child_max = get_node_max_key(table.pager, child);

    uint32_t new_page_num = table.pager.get_unused_page_num();

    bool splitting_root = is_node_root(old_node);

    void* parent;
    void* new_node_ptr;
    if (splitting_root) {
        create_new_root(table, new_page_num);
        parent = table.pager.get_page(table.root_page_num);
        old_page_num = *internal_node_child(parent, 0);
        old_node = table.pager.get_page(old_page_num);
    } else {
        parent = table.pager.get_page(*node_parent(old_node));
        new_node_ptr = table.pager.get_page(new_page_num);
        initialize_internal_node(new_node_ptr);
    }

    uint32_t* old_num_keys = internal_node_num_keys(old_node);

    uint32_t cur_page_num = *internal_node_right_child(old_node);
    void* cur = table.pager.get_page(cur_page_num);

    /* First put right child into new node and set right child of old node to invalid */
    internal_node_insert(table, new_page_num, cur_page_num);
    *node_parent(cur) = new_page_num;
    *internal_node_right_child(old_node) = INVALID_PAGE_NUM;

    /* For each key until you get to the middle key, move the key and the child to the new node */
    for (int i = static_cast<int>(INTERNAL_NODE_MAX_CELLS) - 1;
         i > static_cast<int>(INTERNAL_NODE_MAX_CELLS) / 2; i--) {
        cur_page_num = *internal_node_child(old_node, static_cast<uint32_t>(i));
        cur = table.pager.get_page(cur_page_num);

        internal_node_insert(table, new_page_num, cur_page_num);
        *node_parent(cur) = new_page_num;

        (*old_num_keys)--;
    }

    /* Set child before middle key, which is now the highest key, to be node's right child */
    *internal_node_right_child(old_node) =
        *internal_node_child(old_node, *old_num_keys - 1);
    (*old_num_keys)--;

    /* Determine which of the two nodes should contain the child to be inserted */
    uint32_t max_after_split = get_node_max_key(table.pager, old_node);
    uint32_t destination_page_num =
        child_max < max_after_split ? old_page_num : new_page_num;

    internal_node_insert(table, destination_page_num, child_page_num);
    *node_parent(child) = destination_page_num;

    update_internal_node_key(parent, old_max,
                             get_node_max_key(table.pager, old_node));

    if (!splitting_root) {
        internal_node_insert(table, *node_parent(old_node), new_page_num);
        *node_parent(new_node_ptr) = *node_parent(old_node);
    }
}

Cursor internal_node_find(Table& table, uint32_t page_num, uint32_t key) {
    void* node = table.pager.get_page(page_num);

    uint32_t child_index = internal_node_find_child(node, key);
    uint32_t child_page_num = *internal_node_child(node, child_index);
    void* child = table.pager.get_page(child_page_num);
    switch (get_node_type(child)) {
        case NodeType::LEAF:
            return leaf_node_find(table, child_page_num, key);
        case NodeType::INTERNAL:
            return internal_node_find(table, child_page_num, key);
    }
}
