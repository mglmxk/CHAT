#pragma once
#include "ports/log_sink.hpp"
#include <fstream>
#include <stdexcept>
#include <string>

class FileLogSink final : public ILogSink {
public:
    explicit FileLogSink(const std::string& path) : out_(path, std::ios::app) {
        if (!out_) throw std::runtime_error("Cannot open log file: " + path);
    }
    void append(std::string_view line) override { out_ << line << '\n'; out_.flush(); }
private:
    std::ofstream out_;
};