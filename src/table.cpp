//============================================================================
// TABLE / STORAGE MANAGEMENT
//     Manages how rows are organized into fixed-size pages and stored in memory.
//     Handles allocation, access, and indexing of pages within the table.
//============================================================================


//============================================================================
// Include Statements
//============================================================================
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