#include <iostream>
#include <string>
#include "repl.h"
#include "statement.h"
#include "table.h"
#include "row.h"

/*
------------------------------------------------------------
Function: print_prompt
Purpose : Displays the database prompt to the user.
------------------------------------------------------------
*/
void print_prompt() {
    std::cout << "db > " << std::flush;
}

/*
------------------------------------------------------------
Function: read_input
Purpose : Reads a line of input from the user.
------------------------------------------------------------
*/
std::string read_input() {
    std::string input;
    std::getline(std::cin, input);
    return input;
}

/*
------------------------------------------------------------
Function: do_meta_command
Purpose : Handles special commands that start with a dot (e.g., .exit).
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
Function: run_repl
Purpose : Main read-eval-print loop for the database shell.
Continuously reads commands, prepares them, and executes them.
------------------------------------------------------------
*/
void run_repl() {
    // Open or create the database table
    Table table("test.db");

    while (true) {
        // Display prompt and read user input
        print_prompt();
        std::string input = read_input();

        // Check for meta commands (commands starting with '.')
        if (!input.empty() && input[0] == '.') {
            switch (do_meta_command(input)) {
                case MetaCommandResult::SUCCESS:
                    // Exit command - destructor flushes table to disk
                    return;
                case MetaCommandResult::UNRECOGNIZED_COMMAND:
                    std::cout << "Unrecognized command '" << input << "'\n";
                    continue;
            }
        }

        // Parse the input into a statement
        Statement statement;
        switch (prepare_statement(input, statement)) {
            case PrepareResult::SUCCESS:
                break;
            case PrepareResult::UNRECOGNIZED_STATEMENT:
                std::cout << "Unrecognized keyword at start of '" << input << "'\n";
                continue;
        }

        // Execute the prepared statement
        ExecuteResult result = execute_statement(statement, table);

        // Display result to user
        if (result == ExecuteResult::SUCCESS) {
            std::cout << "Executed.\n";
        } else if (result == ExecuteResult::TABLE_FULL) {
            std::cout << "Error: Table full.\n";
        } else {
            std::cout << "Other Error\n";
        }
    }
}