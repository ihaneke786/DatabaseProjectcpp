#include "cursor.h"
#include "table.h"

/*
------------------------------------------------------------
Function: table_start
Purpose : Creates a cursor positioned at the beginning of the table.
------------------------------------------------------------
*/
Cursor table_start(Table& table) {
    Cursor cursor;
    cursor.table = &table;
    cursor.row_num = 0;
    // Table is empty if there are no rows
    cursor.end_of_table = (table.num_rows == 0);
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
    // Position at the spot where new rows would be inserted
    cursor.row_num = table.num_rows;
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
    return cursor.table->row_slot(cursor.row_num);
}

/*
------------------------------------------------------------
Function: cursor_advance
Purpose : Moves the cursor forward by one row and marks if we've reached the end.
------------------------------------------------------------
*/
void cursor_advance(Cursor& cursor) {
    cursor.row_num++;
    // Mark end of table if we've moved past the last row
    if (cursor.row_num >= cursor.table->num_rows) {
        cursor.end_of_table = true;
    }
}