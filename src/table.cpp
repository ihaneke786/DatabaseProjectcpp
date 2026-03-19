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
// void* is a pointer that can store the memory adress of any data type

void* row_slot(Table* table, uint32_t row_num){
uint32_t page_num = row_num / ROWS_PER_PAGE;
void* page = table->pages[page_num];
    
    // if page does not exist we create a new one
    if(page == nullptr){
        page = table->pages[page_num] = malloc(PAGE_SIZE);
        }
    
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
        return (char*)page + byte_offset;
}



/*
------------------------------------------------------------
Function: new_table
Purpose : Creates a new table on the heap
------------------------------------------------------------
*/
Table* new_table() {
    Table* table = (Table*)malloc(sizeof(Table));

    table->num_rows = 0;

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        table->pages[i] = nullptr;
    }

    return table;
}


/*
------------------------------------------------------------
Function: free_table
Purpose : removes table from heap
------------------------------------------------------------
*/


void free_table(Table* table) {
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        if (table->pages[i] != nullptr) {
            free(table->pages[i]);
        }
    }

    free(table);
}