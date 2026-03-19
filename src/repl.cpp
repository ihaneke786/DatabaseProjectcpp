//============================================================================
// REPL (Read-Eval-Print Loop)
//     Command line interface that lets users enter database commands,
//     execute them, and display the results.
//============================================================================


//============================================================================
// Include statements
//============================================================================
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include "repl.h"
#include "statement.h"
#include "table.h"
#include "row.h"

//============================================================================
// Functions
//============================================================================

/*
------------------------------------------------------------
Function: print prompt
Purpose : just prints db >
------------------------------------------------------------
*/
void print_prompt() {
    std::cout << "db > " << std::flush;
}

/*
------------------------------------------------------------
Function: read input
Purpose : gets input from user, stores it in buffer then returns buffer
------------------------------------------------------------
*/

std::string read_input() {
    std::string input;
    std::getline(std::cin, input);
    return input;
}


/*
------------------------------------------------------------
Function: exit command
Purpose : if user types .exit, the db turns off
------------------------------------------------------------
*/
MetaCommandResult do_meta_command(const std::string& input) {
    if (input == ".exit") {
        return MetaCommandResult::SUCCESS;
    }

    return MetaCommandResult::UNRECOGNIZED_COMMAND;
}


/*
------------------------------------------------------------
Function: run repl function
Purpose : runs the print prompt function, reads user input, checks if user 
input matches a command 
------------------------------------------------------------
*/
void run_repl() {
    // create a table instance
    Table* table = new_table();
    
    while (true) {
        print_prompt();
        std::string input = read_input();

        if (!input.empty() && input[0] == '.') { // checks .exit else unrecognized
        switch (do_meta_command(input)) {
            case MetaCommandResult::SUCCESS:
                free_table(table);   
                return;              

            case MetaCommandResult::UNRECOGNIZED_COMMAND:
                std::cout << "Unrecognized command '" << input << "'\n";
                continue;
        }
}

        Statement statement;
        switch (prepare_statement(input, statement)) {

            case PrepareResult::SUCCESS:
                break;

            case PrepareResult::UNRECOGNIZED_STATEMENT:
                std::cout << "Unrecognized keyword at start of '" << input << "'\n";
                continue;
        }

        ExecuteResult result = execute_statement(&statement, table);

        if (result == ExecuteResult::SUCCESS) {
            std::cout << "Executed.\n" << std::flush;
        }
        else if (result == ExecuteResult::TABLE_FULL) {
            std::cout << "Error: Table full.\n" << std::flush;
        }
        else {
            std::cout << "Other Error\n" << std::flush;
        }
    }

    free_table(table);
}