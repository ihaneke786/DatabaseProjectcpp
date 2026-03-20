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
    Cursor cursor;
    cursor.table = &table;

    // Start at root node (which is currently always a leaf)
    cursor.page_num = table.root_page_num;
    cursor.cell_num = 0;

    // Get the root node page
    void* root_node = table.pager.get_page(table.root_page_num);

    // Check how many cells are in the root
    uint32_t num_cells = LEAF_NODE_NUM_CELLS(root_node);

    // If no cells → end of table
    cursor.end_of_table = (num_cells == 0);

    return cursor;
}

/*
------------------------------------------------------------
Function: table_end
Purpose : Creates a cursor positioned at the end of the table (one past last row).
------------------------------------------------------------
*/
Cursor table_end(Table& table) {
    Cursor cursor;
    cursor.table = &table;

    // Go to root node (currently always a leaf)
    cursor.page_num = table.root_page_num;

    void* root_node = table.pager.get_page(table.root_page_num);
    uint32_t num_cells = LEAF_NODE_NUM_CELLS(root_node);

    // Position cursor at the end (one past last cell)
    cursor.cell_num = num_cells;
    cursor.end_of_table = true;

    return cursor;
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
    if (cursor.cell_num >= LEAF_NODE_NUM_CELLS(node)) {
        cursor.end_of_table = true;
    }
}