#pragma once
#include <fstream>
#include <vector>
#include <memory>

const uint32_t PAGE_SIZE = 4096;
const uint32_t TABLE_MAX_PAGES = 100;

class Pager {
public:
    std::fstream file;
    std::string filename;
    uint32_t file_length;
    std::vector<std::unique_ptr<char[]>> pages;

    Pager(const std::string& filename);
    ~Pager();

    char* get_page(uint32_t page_num);
    void flush(uint32_t num_rows);
};