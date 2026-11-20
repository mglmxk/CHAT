#pragma once
#include <string_view>

class ILogSink {
public:
    virtual ~ILogSink() = default;
    virtual void append(std::string_view line) = 0;
};