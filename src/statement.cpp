//============================================================================
// STATEMENT / PARSER & EXECUTION LAYER
//     Responsible for interpreting user input and converting it into
//     structured commands (Statements) that the database can execute.
//
//     Responsibilities:
//     - Parse input strings into Statement objects
//     - Validate syntax and extract data (id, username, email)
//     - Dispatch execution logic based on statement type (INSERT, SELECT)
//
//     This layer bridges the REPL (user interface) and the storage layer,
//     transforming raw text into structured operations.
//============================================================================



#include "statement.h"
#include <sstream>
#include <iostream>
#include <cstring>


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
        // move data into statement
        statement.row_to_insert.id = id;
        username.copy(statement.row_to_insert.username, sizeof(statement.row_to_insert.username) -1);
        statement.row_to_insert.username[username.size()] = '\0';
        email.copy(statement.row_to_insert.email, sizeof(statement.row_to_insert.email) -1);
        statement.row_to_insert.email[email.size()] = '\0';
        
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
Function: Execute Statement
Purpose : Switch statement to do Insert or Select
------------------------------------------------------------
*/

void execute_statement(const Statement& statement) {

    switch (statement.type) {

        case StatementType::INSERT:
            std::cout << "This is where we would do an insert.\n";
            break;

        case StatementType::SELECT:
            std::cout << "This is where we would do a select.\n";
            break;
    }
}
