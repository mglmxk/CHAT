#pragma once

#include <string>
#include <fstream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <chrono>
#include <ctime>

class AppendOnlyLog {
public:
    explicit AppendOnlyLog(const std::string& filename)
        : stream_(filename, std::ios::app) {
        if (!stream_.is_open()) {
            throw std::runtime_error("Cannot open log file: " + filename);
        }
    }

    AppendOnlyLog(const AppendOnlyLog&) = delete;
    AppendOnlyLog& operator=(const AppendOnlyLog&) = delete;

    AppendOnlyLog(AppendOnlyLog&&) noexcept = default;
    AppendOnlyLog& operator=(AppendOnlyLog&&) noexcept = default;

    ~AppendOnlyLog() {
        if (stream_.is_open()) {
            stream_.flush();
        
        }
    }

    void append(const std::string& entry) {
        if (!stream_.is_open()) {
            throw std::runtime_error("Log file is not open");
        }
        
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        char buf[64];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
        
        stream_ << "[" << buf << "] " << entry << "\n";
        stream_.flush(); 
    }

    const std::string& filename() const { return stream_ ? "" : ""; }

private:
    std::ofstream stream_;
};

struct FileCloser {
    void operator()(FILE* f) const {
        if (f) std::fclose(f);
    }
};

using UniqueFile = std::unique_ptr<FILE, FileCloser>;

inline UniqueFile open_file(const char* path, const char* mode) {
    FILE* f = std::fopen(path, mode);
    if (!f) {
        throw std::runtime_error(std::string("Cannot open file: ") + path);
    }
    return UniqueFile(f);
}