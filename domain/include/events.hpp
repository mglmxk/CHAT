#pragma once
#include <string>
#include <cstdint>
#include <chrono>

struct MessagePosted {
    std::string room, author, text;
    std::uint64_t message_id = 0;
    std::int64_t timestamp_ms = 0;
};
struct UserJoined { std::string room, nick; std::int64_t timestamp_ms = 0; };
struct UserLeft   { std::string room, nick; std::int64_t timestamp_ms = 0; };

inline std::int64_t to_ms(std::chrono::system_clock::time_point tp) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
}