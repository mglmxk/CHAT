#pragma once

#include <chrono>
#include <compare>
#include <cstdint>
#include <string>

class MessageId {
public:
    MessageId(uint64_t sequence, std::chrono::system_clock::time_point timestamp)
        : sequence_(sequence), timestamp_(timestamp) {}

    auto operator<=>(const MessageId&) const = default;

    uint64_t sequence() const { return sequence_; }
    auto timestamp() const { return timestamp_; }

private:
    uint64_t sequence_;
    std::chrono::system_clock::time_point timestamp_;
};

class MessageIdEx {
public:
    MessageIdEx(uint64_t sequence,
                std::chrono::system_clock::time_point timestamp,
                std::string author)
        : sequence_(sequence), timestamp_(timestamp), author_(std::move(author)) {}

    std::strong_ordering operator<=>(const MessageIdEx& other) const {
        if (auto c = sequence_ <=> other.sequence_; c != 0) return c;
        return timestamp_.time_since_epoch() <=> other.timestamp_.time_since_epoch();
        // author_ сознательно НЕ сравнивается
    }

    bool operator==(const MessageIdEx& other) const {
        return sequence_ == other.sequence_ &&
               timestamp_ == other.timestamp_;
        // author_ сознательно НЕ сравнивается
    }

    uint64_t sequence() const { return sequence_; }
    auto timestamp() const { return timestamp_; }
    const std::string& author() const { return author_; }

private:
    uint64_t sequence_;
    std::chrono::system_clock::time_point timestamp_;
    std::string author_;   // вне сравнения
};