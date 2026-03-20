/*
------------------------------------------------------------
Function: Table_start
Purpose : Creates a new cursor at the start of table
------------------------------------------------------------
*/
#include "cursor.h"
#include "table.h"

Cursor table_start(Table& table){
    Cursor cursor;
    cursor.table = &table;
    cursor.row_num = 0;
    cursor.end_of_table = (table.num_rows == 0);

        return cursor;
}


/*
------------------------------------------------------------
Function: Table_end
Purpose : Creates a new cursor at the end of the table
------------------------------------------------------------
*/
Cursor table_end(Table& table){
    Cursor cursor;
    cursor.table = &table;
    cursor.row_num = table.num_rows;
    cursor.end_of_table = true;

        return cursor;
}



/*
------------------------------------------------------------
Function: Cursor value
Purpose : Uses the row_slot function to return a pointer to the position of cursor
------------------------------------------------------------
*/
char* cursor_value(Cursor& cursor) {
    return cursor.table->row_slot(cursor.row_num);
}



/*
------------------------------------------------------------
Function: cursor advance
Purpose : Moves the cursor forward one row, 
also marks if we are at the end of the table
------------------------------------------------------------
*/

void cursor_advance(Cursor& cursor) {
    cursor.row_num++;

    if (cursor.row_num >= cursor.table->num_rows) {
        cursor.end_of_table = true;
    }
}