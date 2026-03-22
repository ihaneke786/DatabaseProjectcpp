#include <iostream>
#include <cstring>
#include <cstdio>
#include "table.h"
#include "row.h"
#include "cursor.h"
#include "statement.h"
#include "btree.h"

/*
------------------------------------------------------------
Function: prepare_statement
Purpose : Parses user input and converts it into a Statement object.
Handles INSERT and SELECT commands.
------------------------------------------------------------
*/
PrepareResult prepare_statement(const std::string& input, Statement& statement) {
    if (input.substr(0, 6) == "insert") {
        statement.type = StatementType::INSERT;
        int id;
        char username[32], email[32];
        if (sscanf(input.c_str(), "insert %d %31s %31s", &id, username, email) == 3) {
            statement.row_to_insert.id = id;
            strcpy(statement.row_to_insert.username, username);
            strcpy(statement.row_to_insert.email, email);
            return PrepareResult::SUCCESS;
        }
    } else if (input == "select") {
        statement.type = StatementType::SELECT;
        return PrepareResult::SUCCESS;
    }
    return PrepareResult::UNRECOGNIZED_STATEMENT;
}

/*
------------------------------------------------------------
Function: execute_insert
Purpose : Inserts a new row into the table at the end.
Returns error if table is full.
------------------------------------------------------------
*/
ExecuteResult execute_insert(Statement& statement, Table& table) {
    void* node = table.pager.get_page(table.root_page_num);

    uint32_t num_cells = *leaf_node_num_cells(node);

    Row& row_to_insert = statement.row_to_insert;
    uint32_t key_to_insert = row_to_insert.id;

    // Find correct position
    Cursor cursor = table_find(table, key_to_insert);

    // Check for duplicate
    if (cursor.cell_num < num_cells) {
        uint32_t key_at_index =
            *leaf_node_key(node, cursor.cell_num);

        if (key_at_index == key_to_insert) {
            return ExecuteResult::DUPLICATE_KEY;
        }
    }

    // Insert at correct position
    leaf_node_insert(cursor, key_to_insert, row_to_insert);

    return ExecuteResult::SUCCESS;
}

/*
------------------------------------------------------------
Function: execute_select
Purpose : Iterates through all rows in the table and prints them.
------------------------------------------------------------
*/
ExecuteResult execute_select(Statement& /*statement*/, Table& table) {
    Row tempRow;
    // Start cursor at the beginning of the table
    Cursor cursor = table_start(table);

    // Iterate through all rows and print them
    while (!cursor.end_of_table) {
        // Convert binary format back to Row struct
        deserialize_row(cursor_value(cursor), &tempRow);
        // Print the row data
        std::cout << tempRow.id << " "
                  << tempRow.username << " "
                  << tempRow.email << "\n";
        // Move to next row
        cursor_advance(cursor);
    }

    return ExecuteResult::SUCCESS;
}


/*
------------------------------------------------------------
Function: execute_statement
Purpose : Routes a prepared statement to the appropriate execution function.
------------------------------------------------------------
*/
ExecuteResult execute_statement(Statement& statement, Table& table) {
    switch (statement.type) {
        case StatementType::INSERT:
            return execute_insert(statement, table);
        case StatementType::SELECT:
            return execute_select(statement, table);
        default:
            return ExecuteResult::SUCCESS;
    }
}