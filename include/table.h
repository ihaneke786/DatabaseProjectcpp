//============================================================================
// Include Statements
//============================================================================
#pragma once
#include <cstdint>
#include "row.h"

//============================================================================
// Constants for page table
// Keeps track of the number of rows
//============================================================================

const uint32_t PAGE_SIZE = 4096; // 4kb
const uint32_t TABLE_MAX_PAGES = 100;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE/ ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;



//============================================================================
// Structs
//============================================================================

struct Table{
    uint32_t num_rows;
    void* pages[TABLE_MAX_PAGES]; // pointers to pages
};
