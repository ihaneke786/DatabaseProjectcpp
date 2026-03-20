#include <iostream>
#include <string>
#include "repl.h"
#include "statement.h"
#include "table.h"
#include "row.h"

//============================================================================
// Print prompt
//============================================================================
void print_prompt() {
    std::cout << "db > " << std::flush;
}

//============================================================================
// Read input
//============================================================================
std::string read_input() {
    std::string input;
    std::getline(std::cin, input);
    return input;
}

//============================================================================
// Meta commands (.exit)
//============================================================================
MetaCommandResult do_meta_command(const std::string& input) {
    if (input == ".exit") {
        return MetaCommandResult::SUCCESS;
    }

    return MetaCommandResult::UNRECOGNIZED_COMMAND;
}

//============================================================================
// Run REPL
//============================================================================
void run_repl() {

    // ✅ C++ object (NOT pointer)
    Table table("test.db");

    while (true) {
        print_prompt();
        std::string input = read_input();

        // Handle meta commands
        if (!input.empty() && input[0] == '.') {
            switch (do_meta_command(input)) {

                case MetaCommandResult::SUCCESS:
                    return;  // ✅ triggers destructor automatically

                case MetaCommandResult::UNRECOGNIZED_COMMAND:
                    std::cout << "Unrecognized command '" << input << "'\n";
                    continue;
            }
        }

        // Prepare statement
        Statement statement;
        switch (prepare_statement(input, statement)) {

            case PrepareResult::SUCCESS:
                break;

            case PrepareResult::UNRECOGNIZED_STATEMENT:
                std::cout << "Unrecognized keyword at start of '" << input << "'\n";
                continue;
        }

        // Execute statement
        ExecuteResult result = execute_statement(statement, table);

        if (result == ExecuteResult::SUCCESS) {
            std::cout << "Executed.\n";
        } 
        else if (result == ExecuteResult::TABLE_FULL) {
            std::cout << "Error: Table full.\n";
        } 
        else {
            std::cout << "Other Error\n";
        }
    }
}