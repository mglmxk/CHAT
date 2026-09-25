#pragma once

#include <string>
#include <chrono>

class Message {
public:
    Message(std::string author, std::string text)
        : author_(std::move(author))
        , text_(std::move(text))
        , timestamp_(std::chrono::system_clock::now()) {}

    const std::string& author() const { 
        return author_; 
    }
    const std::string& text() const { 
        return text_; 
    }
    auto timestamp() const { 
        return timestamp_; 
    }

private:
    std::string author_;
    std::string text_;
    std::chrono::system_clock::time_point timestamp_;
};