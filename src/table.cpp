#include <iostream>
#include "table.h"
#include <cstdlib>
#include <cstddef>

/*
------------------------------------------------------------
Function: row_slot
Purpose : Calculates and returns the memory address where a specific row is stored.
------------------------------------------------------------
*/
char* Table::row_slot(uint32_t row_num) {
    // Determine which page this row is on
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    char* page = pager.get_page(page_num);
    
    // Find the offset of this row within its page
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    
    // Return pointer to the exact location of this row in memory
    return page + byte_offset;
}

/*
------------------------------------------------------------
Function: Table Constructor
Purpose : Initializes a new table by opening the database file
and calculating the current number of rows stored.
------------------------------------------------------------
*/
Table::Table(const std::string& filename)
    : pager(filename) {
    // Calculate number of rows based on file size
    num_rows = pager.file_length / ROW_SIZE;
}

/*
------------------------------------------------------------
Function: Table Destructor
Purpose : Flushes all in-memory pages to disk when the table is destroyed.
------------------------------------------------------------
*/
Table::~Table() {
    pager.flush(num_rows);
}

Table* new_table(const std::string& filename) {
    return new Table(filename);
}

void free_table(Table* table) {
    delete table;
}


