#include "pager.h"
#include <iostream>
#include <cstring>

Pager::Pager(const std::string& filename) {
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        file.open(filename, std::ios::out | std::ios::binary);
        file.close();
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }

    if (!file.is_open()) {
        std::cerr << "Unable to open file\n";
        exit(EXIT_FAILURE);
    }

    file.seekg(0, std::ios::end);
    file_length = file.tellg();
    file.seekg(0, std::ios::beg);

    if (file_length % PAGE_SIZE != 0) {
        std::cerr << "DB file is not a whole number of pages\n";
        exit(EXIT_FAILURE);
    }

    pages.resize(TABLE_MAX_PAGES, nullptr);
    num_pages = file_length / PAGE_SIZE;
}

Pager::~Pager() {
    for (auto page : pages) {
        delete[] page;
    }
    if (file.is_open()) file.close();
}

void Pager::flush(uint32_t page_num) {
    if (!pages[page_num]) return;

    file.seekp(page_num * PAGE_SIZE);
    file.write(pages[page_num], PAGE_SIZE);
    file.flush();
}

char* Pager::get_page(uint32_t page_num) {
    if (page_num >= TABLE_MAX_PAGES) {
        std::cerr << "Page number out of bounds\n";
        exit(EXIT_FAILURE);
    }

    if (!pages[page_num]) {
        pages[page_num] = new char[PAGE_SIZE];
        memset(pages[page_num], 0, PAGE_SIZE);

        if (page_num >= num_pages) {
            num_pages = page_num + 1;
        } else {
            file.seekg(page_num * PAGE_SIZE);
            file.read(pages[page_num], PAGE_SIZE);
            file.clear();
        }
    }

    return pages[page_num];
}