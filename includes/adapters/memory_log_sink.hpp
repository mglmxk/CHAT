#pragma once
#include "ports/log_sink.hpp"
#include <vector>
#include <string>

class MemoryLogSink final : public ILogSink {
public:
    void append(std::string_view line) override { lines_.emplace_back(line); }
    const std::vector<std::string>& lines() const { return lines_; }
    void clear() { lines_.clear(); }
private:
    std::vector<std::string> lines_;
};