//============================================================================
// Include statements
//============================================================================
#pragma once
#include <cstdint>


//============================================================================
// Constants
//============================================================================

// get sizes in bytes for variables in row
const uint32_t ID_SIZE = sizeof(int);
const uint32_t USERNAME_SIZE = 32;
const uint32_t EMAIL_SIZE = 32;
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;



struct Row {
    int id;
    char username[USERNAME_SIZE];
    char email[EMAIL_SIZE];
};


void serialize_row(Row* source, void* destination);
void deserialize_row(void* source, Row* destination);
