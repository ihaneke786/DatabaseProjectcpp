//============================================================================
// Include statements
//============================================================================
#pragma once
#include <string>
#include "row.h"

class Table;
//============================================================================
// Enums
//============================================================================


// Enums related to statements
enum class PrepareResult {
    SUCCESS,
    UNRECOGNIZED_STATEMENT
};

enum class StatementType {
    INSERT,
    SELECT
};


enum class ExecuteResult {
    SUCCESS,
    TABLE_FULL
};


// Statement struct
struct Statement {
    StatementType type;
    Row row_to_insert;
};

// Function declarations
PrepareResult prepare_statement(const std::string& input, Statement& statement);
ExecuteResult execute_statement(Statement& statement, Table& table);