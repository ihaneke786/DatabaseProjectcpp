#include <iostream>
#include "table.h"
#include <cstdlib>
#include <cstddef>
#include "btree.h"
/*
------------------------------------------------------------
Function: Table Constructor
Purpose : Initializes a new table by opening the database file
and calculating the current number of rows stored.
------------------------------------------------------------
*/
Table::Table(const std::string& filename) : pager(filename) {
    root_page_num = 0;
    if (pager.num_pages == 0) {
        char* root_node = pager.get_page(0);
        initialize_leaf_node(root_node);
        SET_ROOT(root_node, true);
    }
}

/*
------------------------------------------------------------
Function: Table Destructor
Purpose : Flushes all in-memory pages to disk when the table is destroyed.
------------------------------------------------------------
*/
Table::~Table() {
    for (uint32_t i = 0; i < pager.num_pages; i++) {
        if (pager.pages[i]) {
            pager.flush(i);
        }
    }
}

Table* new_table(const std::string& filename) {
    return new Table(filename);
}

void free_table(Table* table) {
    delete table;
}


