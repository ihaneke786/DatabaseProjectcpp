//============================================================================
// STATEMENT / PARSER & EXECUTION LAYER
//     Converts raw user input into structured commands and routes them to
//     the appropriate execution logic (INSERT or SELECT).
//============================================================================



//============================================================================
// Include statements
//============================================================================

#include <sstream>
#include <iostream>
#include <cstring>
#include "table.h"
#include "row.h"
#include "statement.h"

/*
------------------------------------------------------------
Function: Prepare Statment
Purpose : Checks if user inputs insert or select else returns UNRECOGNIZED_STATEMENT
------------------------------------------------------------
*/
PrepareResult prepare_statement(const std::string& input, Statement& statement) {

    if (input.rfind("insert", 0) == 0) {
        statement.type = StatementType::INSERT;
        // we parse the user input to extract id, username and email
        std::istringstream iss(input);
        int id;
        std::string keyword, username, email;
        iss >> keyword >> id >> username >> email;
            if(iss.fail()){
                return PrepareResult::UNRECOGNIZED_STATEMENT;
            }
        // move data into statement struct
        //id
        statement.row_to_insert.id = id;
        //username
        strncpy(statement.row_to_insert.username, username.c_str(), sizeof(statement.row_to_insert.username) - 1);
        statement.row_to_insert.username[sizeof(statement.row_to_insert.username) - 1] = '\0';
        // email
        strncpy(statement.row_to_insert.email, email.c_str(), sizeof(statement.row_to_insert.email) - 1);
        statement.row_to_insert.email[sizeof(statement.row_to_insert.email) - 1] = '\0';
        
        return PrepareResult::SUCCESS;
    }

    if (input == "select") {
        statement.type = StatementType::SELECT;
        return PrepareResult::SUCCESS;
    }

    return PrepareResult::UNRECOGNIZED_STATEMENT;
}



/*
------------------------------------------------------------
Function: Execute Statement(s)
Purpose : Switch statement to do Insert or Select
------------------------------------------------------------
*/

// Insert command: this will insert row into table
ExecuteResult execute_insert(Statement* statement, Table* table){
    // see if table is full
    if(table->num_rows == TABLE_MAX_ROWS){
        return ExecuteResult::TABLE_FULL;
    }

    serialize_row(&(statement->row_to_insert),row_slot(table, table->num_rows));
    table->num_rows +=1;
    return ExecuteResult::SUCCESS;
}



// Select command: This will print out results for select statement
ExecuteResult execute_select(Statement* statement, Table* table){
    Row tempRow;
    for(uint32_t i =0; i < table->num_rows; i++){
        deserialize_row(row_slot(table, i), &tempRow);
        std::cout << tempRow.id <<
        " " << tempRow.username <<
        " " << tempRow.email << "\n";
    }
    return ExecuteResult::SUCCESS;
}


// Switch statement for commands
ExecuteResult execute_statement(Statement* statement, Table* table) {

    switch (statement->type) {

        case StatementType::INSERT:
            return execute_insert(statement, table);

        case StatementType::SELECT:
            return execute_select(statement, table);

        default:
            return ExecuteResult::SUCCESS;
    }
}