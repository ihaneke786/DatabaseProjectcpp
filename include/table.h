//============================================================================
// Include Statements
//============================================================================
#pragma once
#include <cstdint>
#include "row.h"
#include "pager.h"


//============================================================================
// Structs
//============================================================================

class Table {
public:
    Pager pager;
    uint32_t root_page_num;

    Table(const std::string& filename);
    ~Table();

    // char* row_slot(uint32_t row_num); // Removed for B-Tree implementation
};


//============================================================================
// Function declarations
//============================================================================
Table* new_table(const std::string& filename);
void free_table(Table* table);
