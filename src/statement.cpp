#include <sstream>
#include <iostream>
#include <cstring>
#include "table.h"
#include "row.h"
#include "cursor.h"
#include "statement.h"

/*
------------------------------------------------------------
Function: prepare_statement
Purpose : Parses user input and converts it into a Statement object.
Handles INSERT and SELECT commands.
------------------------------------------------------------
*/
PrepareResult prepare_statement(const std::string& input, Statement& statement) {
    // Handle INSERT command
    if (input.rfind("insert", 0) == 0) {
        statement.type = StatementType::INSERT;
        
        // Parse the input to extract id, username, and email
        std::istringstream iss(input);
        int id;
        std::string keyword, username, email;
        iss >> keyword >> id >> username >> email;
        
        // Check if parsing succeeded
        if (iss.fail()) {
            return PrepareResult::UNRECOGNIZED_STATEMENT;
        }
        
        // Store parsed values in the statement
        statement.row_to_insert.id = id;
        
        // Copy username with null termination safety
        strncpy(statement.row_to_insert.username, username.c_str(), sizeof(statement.row_to_insert.username) - 1);
        statement.row_to_insert.username[sizeof(statement.row_to_insert.username) - 1] = '\0';
        
        // Copy email with null termination safety
        strncpy(statement.row_to_insert.email, email.c_str(), sizeof(statement.row_to_insert.email) - 1);
        statement.row_to_insert.email[sizeof(statement.row_to_insert.email) - 1] = '\0';
        
        return PrepareResult::SUCCESS;
    }

    // Handle SELECT command
    if (input == "select") {
        statement.type = StatementType::SELECT;
        return PrepareResult::SUCCESS;
    }

    // Unknown command
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
    // Check if table is full before inserting
    if (table.num_rows == TABLE_MAX_ROWS) {
        return ExecuteResult::TABLE_FULL;
    }

    // Get cursor at end of table to insert new row
    Cursor cursor = table_end(table);
    // Convert row struct to binary format and store at cursor position
    serialize_row(&statement.row_to_insert, cursor_value(cursor));
    // Increment row count
    table.num_rows += 1;

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