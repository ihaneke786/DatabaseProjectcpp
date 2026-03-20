//============================================================================
// Include Statements
//============================================================================
#pragma once
#include <cstdint>
#include "row.h"
#include "pager.h"
//============================================================================
// Constants for page table
// Keeps track of the number of rows
//============================================================================

const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;



//============================================================================
// Structs
//============================================================================

class Table {
public:
    Pager pager;
    uint32_t num_rows;

    Table(const std::string& filename);
    ~Table();

    char* row_slot(uint32_t row_num);
};


//============================================================================
// Function declarations
//============================================================================
Table* new_table(const std::string& filename);
void free_table(Table* table);
