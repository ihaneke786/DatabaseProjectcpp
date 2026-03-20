#pragma once
#include "table.h"

struct Cursor {
    Table* table;
    uint32_t page_num;
    uint32_t cell_num;
    bool end_of_table;
};

// functions we need elsewhere
Cursor table_start(Table& table);
Cursor table_end(Table& table);
char* cursor_value(Cursor& cursor);
void cursor_advance(Cursor& cursor) ;