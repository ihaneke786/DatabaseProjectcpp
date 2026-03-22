#pragma once
#include <cstdint>
#include <fstream>
#include <vector>

const uint32_t PAGE_SIZE = 4096;
const uint32_t TABLE_MAX_PAGES = 100;

class Pager {
public:
    std::fstream file;
    uint32_t file_length = 0;
    std::vector<char*> pages;
    uint32_t num_pages = 0;

    Pager(const std::string& filename);
    ~Pager();

    char* get_page(uint32_t page_num);
    void flush(uint32_t page_num);

    /*
     * Until we start recycling free pages, new pages will always
     * go onto the end of the database file
     */
    uint32_t get_unused_page_num() const { return num_pages; }
};