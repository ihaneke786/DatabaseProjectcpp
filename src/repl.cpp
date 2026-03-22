#include <iostream>
#include <string>
#include "repl.h"
#include "statement.h"
#include "table.h"
#include "row.h"
#include "btree.h"

void print_prompt() { std::cout << "db > " << std::flush; }

std::string read_input() {
    std::string input;
    if (!std::getline(std::cin, input)) return "";
    return input;
}

MetaCommandResult do_meta_command(const std::string& input, Table& table) {
    if (input == ".exit") return MetaCommandResult::SUCCESS;
    if (input == ".constants") {
        print_constants();
        return MetaCommandResult::SUCCESS;
    }
    if (input == ".btree") {
        std::cout << "Tree:\n";
        print_tree(table.pager, table.root_page_num, 0);
        return MetaCommandResult::SUCCESS;
    }
    return MetaCommandResult::UNRECOGNIZED_COMMAND;
}

void run_repl() {
    Table table("test.db");
    while (true) {
        print_prompt();
        std::string input = read_input();
        if (input.empty()) break;

        if (input[0] == '.') {
            if (do_meta_command(input, table) == MetaCommandResult::SUCCESS) return;
            std::cout << "Unrecognized command '" << input << "'\n";
            continue;
        }

        Statement statement;
        if (prepare_statement(input, statement) != PrepareResult::SUCCESS) {
            std::cout << "Unrecognized keyword at start of '" << input << "'\n";
            continue;
        }

        ExecuteResult result = execute_statement(statement, table);
        if (result == ExecuteResult::SUCCESS) {
            std::cout << "Executed.\n";
        } 
        else if (result == ExecuteResult::DUPLICATE_KEY) {
            std::cout << "Error: Duplicate key.\n";
        } 
        else if (result == ExecuteResult::TABLE_FULL) {
            std::cout << "Error: Table full.\n";
        } 
        else {
            std::cout << "Other Error\n";
        }
    }
}