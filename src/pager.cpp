#include "pager.h"
#include <iostream>
#include "table.h"
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

/*
------------------------------------------------------------
Function: Pager Constructor
Purpose : Opens the database file and keeps track of its size.
Also initializes the page cache to null pointers.
------------------------------------------------------------
*/
Pager::Pager(const std::string& filename)
    : filename(filename) {

    // Try to open existing file in read/write binary mode
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

    // If file doesn't exist, create it by opening in write mode, then reopen in read/write mode
    if (!file.is_open()) {
        file.clear();
        file.open(filename, std::ios::out | std::ios::binary);
        file.close();
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }

    // If file still cannot be opened, exit with error
    if (!file.is_open()) {
        std::cerr << "Unable to open file\n";
        exit(EXIT_FAILURE);
    }

    // Move file pointer to end to determine file size in bytes
    file.seekg(0, std::ios::end);
    file_length = static_cast<uint32_t>(file.tellg());
    
    // Initialize page cache vector with empty pointers
    pages.resize(TABLE_MAX_PAGES);
}

/*
------------------------------------------------------------
Function: Pager Destructor
Purpose : Closes the database file when the Pager object is destroyed.
------------------------------------------------------------
*/
Pager::~Pager() {
    if (file.is_open()) {
        file.close();
    }
}


/*
------------------------------------------------------------
Function: flush
Purpose : Writes all in-memory pages to disk and truncates the file to the correct size.
------------------------------------------------------------
*/
void Pager::flush(uint32_t num_rows) {
    // Calculate number of full pages and any remaining rows in a partial page
    uint32_t num_full_pages = num_rows / ROWS_PER_PAGE;
    uint32_t num_additional_rows = num_rows % ROWS_PER_PAGE;
    
    // Determine total number of pages to write (add 1 if partial page exists)
    uint32_t pages_to_write = num_full_pages + (num_additional_rows > 0 ? 1 : 0);

    // Write each page to disk at its correct position
    for (uint32_t i = 0; i < pages_to_write; i++) {
        // Skip pages that haven't been allocated
        if (!pages[i]) continue;

        // Seek to the correct position for this page in the file
        file.seekp(i * PAGE_SIZE);
        
        // For the last partial page, only write the additional rows; otherwise write full page
        if (i == num_full_pages && num_additional_rows > 0) {
            uint32_t bytes_to_write = num_additional_rows * ROW_SIZE;
            file.write(pages[i].get(), bytes_to_write);
        } else {
            file.write(pages[i].get(), PAGE_SIZE);
        }
    }

    // Ensure all buffered data is written to disk
    file.flush();

    // Use ftruncate to set file size to exact number of bytes (for cleanup of unused space)
    off_t target_size = static_cast<off_t>(num_rows * ROW_SIZE);
    int fd = open(filename.c_str(), O_RDWR);
    
    if (fd == -1) {
        std::cerr << "Unable to open file for truncation\n";
        exit(EXIT_FAILURE);
    }
    
    // Truncate file to the exact size needed for all rows
    if (ftruncate(fd, target_size) != 0) {
        std::cerr << "Unable to truncate file\n";
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    close(fd);
}
/*
------------------------------------------------------------
Function: get_page
Purpose : Returns a pointer to a page, loading it from disk if necessary.
Allocates and initializes new pages if they don't exist yet.
------------------------------------------------------------
*/
char* Pager::get_page(uint32_t page_num) {
    // Validate that page number is within bounds
    if (page_num >= TABLE_MAX_PAGES) {
        std::cerr << "Page number out of bounds: " << page_num << "\n";
        exit(EXIT_FAILURE);
    }

    // If page hasn't been cached yet, allocate and load it
    if (!pages[page_num]) {
        // Allocate new page and zero it out
        pages[page_num] = std::make_unique<char[]>(PAGE_SIZE);
        memset(pages[page_num].get(), 0, PAGE_SIZE);

        // Calculate number of pages currently on disk using ceiling division
        uint32_t num_pages = (file_length + PAGE_SIZE - 1) / PAGE_SIZE;

        // If page exists on disk, load it; if not, leave it zeroed (new page)
        if (page_num < num_pages) {
            file.seekg(page_num * PAGE_SIZE);
            file.read(pages[page_num].get(), PAGE_SIZE);
            // Clear error flags that may be set if we read past EOF
            file.clear();

            if (file.bad()) {
                std::cerr << "Error reading file\n";
                exit(EXIT_FAILURE);
            }
        }
    }

    // Return raw pointer to the cached page
    return pages[page_num].get();
}