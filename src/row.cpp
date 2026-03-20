#include "row.h"
#include <cstring>

/*
------------------------------------------------------------
Function: serialize_row
Purpose : Converts a Row struct into compact binary format for storage on disk.
Packs id, username, and email into tightly packed bytes.
------------------------------------------------------------
*/
void serialize_row(Row* source, void* destination) {
    // Clear the destination buffer
    memset(destination, 0, ROW_SIZE);
    // Copy id at its designated offset
    memcpy((char*)destination + ID_OFFSET, &(source->id), ID_SIZE);
    // Copy username at its designated offset
    memcpy((char*)destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
    // Copy email at its designated offset
    memcpy((char*)destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
}

/*
------------------------------------------------------------
Function: deserialize_row
Purpose : Converts compact binary format back into a Row struct.
Extracts id, username, and email from tightly packed bytes.
------------------------------------------------------------
*/
void deserialize_row(void* source, Row* destination) {
    // Extract id from its designated offset
    memcpy(&(destination->id), (char*)source + ID_OFFSET, ID_SIZE);
    // Extract username from its designated offset
    memcpy(destination->username, (char*)source + USERNAME_OFFSET, USERNAME_SIZE);
    // Extract email from its designated offset
    memcpy(destination->email, (char*)source + EMAIL_OFFSET, EMAIL_SIZE);
}
