//============================================================================
// Include statements
//============================================================================
#pragma once
#include <string>
#include "table.h"


//============================================================================
// Enums
//============================================================================
enum class MetaCommandResult {
    SUCCESS,
    UNRECOGNIZED_COMMAND
};
// function declarations
MetaCommandResult do_meta_command(const std::string& input, Table& table);

// run the front end
void run_repl();