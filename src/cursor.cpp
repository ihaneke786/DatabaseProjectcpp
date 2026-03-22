#include "cursor.h"
#include "table.h"
#include "btree.h"
/*
------------------------------------------------------------
Function: table_start
Purpose : Creates a cursor positioned at the beginning of the table.
------------------------------------------------------------
*/
Cursor table_start(Table& table) {
    Cursor cursor = table_find(table, 0);

    void* node = table.pager.get_page(cursor.page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);
    cursor.end_of_table = (num_cells == 0);

    return cursor;
}


// Searches tree for every given key
Cursor table_find(Table& table, uint32_t key) {
    uint32_t root_page_num = table.root_page_num;
    void* root_node = table.pager.get_page(root_page_num);

    if (get_node_type(root_node) == NodeType::LEAF) {
        return leaf_node_find(table, root_page_num, key);
    } else {
        return internal_node_find(table, root_page_num, key);
    }
}

/*
------------------------------------------------------------
Function: cursor_value
Purpose : Returns a pointer to the row data at the cursor's current position.
------------------------------------------------------------
*/
char* cursor_value(Cursor& cursor) {
    uint32_t page_num = cursor.page_num;
    void* page = cursor.table->pager.get_page(page_num);
    return (char*)LEAF_NODE_VALUE(page, cursor.cell_num);
}


/*
------------------------------------------------------------
Function: cursor_advance
Purpose : Moves the cursor forward by one row and marks if we've reached the end.
------------------------------------------------------------
*/
void cursor_advance(Cursor& cursor) {
    uint32_t page_num = cursor.page_num;
    void* node = cursor.table->pager.get_page(page_num);
    cursor.cell_num++;
    if (cursor.cell_num >= *leaf_node_num_cells(node)) {
        /* Advance to next leaf node */
        uint32_t next_page_num = *leaf_node_next_leaf(node);
        if (next_page_num == 0) {
            /* This was rightmost leaf */
            cursor.end_of_table = true;
        } else {
            cursor.page_num = next_page_num;
            cursor.cell_num = 0;
        }
    }
}