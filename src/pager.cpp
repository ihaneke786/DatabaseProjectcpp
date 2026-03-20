#include "pager.h"
#include <iostream>
#include "table.h"
#include <cstring>
#include <unistd.h>
#include <fcntl.h>


/*
------------------------------------------------------------
Function: pager open function(Constructor)
Purpose : Opens the database file and keeps track of its size.
Also initializes the page cache to null pointers.
------------------------------------------------------------
*/

Pager::Pager(const std::string& filename)
    : filename(filename) {

    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

    // If file doesn't exist, create it
    if (!file.is_open()) {
        file.clear();
        file.open(filename, std::ios::out | std::ios::binary); // out means output(write to) and binary means treat as binary
        file.close();

        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }

    if (!file.is_open()) {
        std::cerr << "Unable to open file\n";
        exit(EXIT_FAILURE);
    }

    // Get file length
    file.seekg(0, std::ios::end); // This makes seek go to end of file to get final position to determine length
    file_length = static_cast<uint32_t>(file.tellg()); // tellg gets current position of file pointer, since it is at end, gives us total bytes

    // Initialize page cache
    pages.resize(TABLE_MAX_PAGES);
}

// Pager Destructor
Pager::~Pager() {
    if (file.is_open()) {
        file.close();
    }
}


/*
------------------------------------------------------------
Function: flush
Purpose : writes the in-memory pages to disk
------------------------------------------------------------
*/

void Pager::flush(uint32_t num_rows) {
    uint32_t num_full_pages = num_rows / ROWS_PER_PAGE;
    uint32_t num_additional_rows = num_rows % ROWS_PER_PAGE;

    uint32_t pages_to_write = num_full_pages + (num_additional_rows > 0 ? 1 : 0); // finds amount of pages to write to disk, if additional rows add page

    for (uint32_t i = 0; i < pages_to_write; i++) {
        if (!pages[i]) continue;

        file.seekp(i * PAGE_SIZE); // This moves file pointer to current page we to move to disk
        
        if (i == num_full_pages && num_additional_rows > 0) { // this case for writing a partial page
            uint32_t bytes_to_write = num_additional_rows * ROW_SIZE;
            file.write(pages[i].get(), bytes_to_write);
        } 
        else { // this case for writing a whole page
            file.write(pages[i].get(), PAGE_SIZE);
        }
    }

    file.flush(); // writes data to disk
    file.close(); // closes file

    off_t target_size = static_cast<off_t>(num_rows * ROW_SIZE);
    int fd = open(filename.c_str(), O_RDWR);
    
    if (fd == -1) {
        std::cerr << "Unable to open file for truncation\n";
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, target_size) != 0) {
        std::cerr << "Unable to truncate file\n";
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
}
/*
------------------------------------------------------------
Function: get page function
Purpose : checks if page exists, if on memory, allocate,
if on disk, load it, cache the page and then return the pointer.
------------------------------------------------------------
*/
char* Pager::get_page(uint32_t page_num) {

    // 1. Bounds check
    if (page_num >= TABLE_MAX_PAGES) {
        std::cerr << "Page number out of bounds: " << page_num << "\n";
        exit(EXIT_FAILURE);
    }

    // 2. Cache miss → allocate
    if (!pages[page_num]) {

        pages[page_num] = std::make_unique<char[]>(PAGE_SIZE);
        memset(pages[page_num].get(), 0, PAGE_SIZE);

        // 3. Figure out how many pages exist in file
        uint32_t num_pages = file_length / PAGE_SIZE;

        if (file_length % PAGE_SIZE != 0) {
            num_pages += 1;
        }

        // 4. If page exists on disk → load it
        if (page_num < num_pages) {

            file.seekg(page_num * PAGE_SIZE);
            file.read(pages[page_num].get(), PAGE_SIZE);

            // Clear the failbit and eof bit that may have been set if we read past EOF
            file.clear();

            if (file.bad()) {
                std::cerr << "Error reading file\n";
                exit(EXIT_FAILURE);
            }
        }
        // else: leave page as empty (new page)
    }

    // 5. Return raw pointer
    return pages[page_num].get();
}