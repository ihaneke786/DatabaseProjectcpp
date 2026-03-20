//============================================================================
// TABLE / STORAGE MANAGEMENT
//     Manages how rows are organized into fixed-size pages and stored in memory.
//     Handles allocation, access, and indexing of pages within the table.
//============================================================================


//============================================================================
// Include Statements
//============================================================================
#include <iostream>
#include "table.h"
#include <cstdlib>   // malloc, free
#include <cstddef>   // NULL (or use nullptr)

/*
------------------------------------------------------------
Function: row slot
Purpose : This function finds out where to read/write memory for a row.
------------------------------------------------------------
*/

char* Table::row_slot(uint32_t row_num) {
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    char* page = pager.get_page(page_num);

    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;

    return page + byte_offset;
}



/*
------------------------------------------------------------
Function: Table constructor
Purpose : Creates a new table on the heap
------------------------------------------------------------
*/
Table::Table(const std::string& filename)
    : pager(filename)   // calls Pager constructor
{
    num_rows = pager.file_length / ROW_SIZE;
}

Table::~Table() {
    pager.flush(num_rows);
}

Table* new_table(const std::string& filename) {
    return new Table(filename);
}

void free_table(Table* table) {
    delete table;
}


