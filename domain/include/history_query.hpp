#pragma once
#include <string>
#include <cstddef>

struct HistoryQuery {
    std::string author;  
    size_t limit = 0;    
};