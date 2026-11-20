#pragma once
#include <chrono>
#include <cstdint>
#include <compare>
#include <string>
#include <utility>

class MessageId {
public:
    using time_point = std::chrono::system_clock::time_point;
    MessageId(std::uint64_t seq, time_point ts) : sequence_(seq), timestamp_(ts) {}
    std::uint64_t sequence() const { return sequence_; }
    time_point timestamp() const { return timestamp_; }
    auto operator<=>(const MessageId&) const = default;
private:
    std::uint64_t sequence_;
    time_point timestamp_;
};

class MessageIdEx {
public:
    using time_point = std::chrono::system_clock::time_point;
    MessageIdEx(std::uint64_t seq, time_point ts, std::string author)
        : sequence_(seq), timestamp_(ts), author_(std::move(author)) {}
    std::strong_ordering operator<=>(const MessageIdEx& o) const {
        if (auto c = sequence_ <=> o.sequence_; c != 0) return c;
        return timestamp_ <=> o.timestamp_;
    }
    bool operator==(const MessageIdEx& o) const {
        return sequence_ == o.sequence_ && timestamp_ == o.timestamp_;
    }
private:
    std::uint64_t sequence_;
    time_point timestamp_;
    std::string author_;
};