#pragma once
#include <string>

struct EventEnvelope {
    std::string type;     
    std::string key;      
    std::string payload;  
};