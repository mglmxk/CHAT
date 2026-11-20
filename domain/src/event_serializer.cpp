#include "event_serializer.hpp"
#include <string>

namespace {
std::string esc(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (unsigned char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:
                if (c < 0x20) { out += "\\u00"; out += "0123456789abcdef"[c >> 4]; out += "0123456789abcdef"[c & 15]; }
                else out += static_cast<char>(c);
        }
    }
    return out;
}
std::string q(const std::string& s) { return "\"" + esc(s) + "\""; }
}

EventEnvelope to_envelope(const MessagePosted& e) {
    return {"MessagePosted", e.room,
        "{\"room\":" + q(e.room) + ",\"author\":" + q(e.author) +
        ",\"text\":" + q(e.text) + ",\"message_id\":" + std::to_string(e.message_id) +
        ",\"timestamp_ms\":" + std::to_string(e.timestamp_ms) + "}"};
}
EventEnvelope to_envelope(const UserJoined& e) {
    return {"UserJoined", e.room,
        "{\"room\":" + q(e.room) + ",\"nick\":" + q(e.nick) +
        ",\"timestamp_ms\":" + std::to_string(e.timestamp_ms) + "}"};
}
EventEnvelope to_envelope(const UserLeft& e) {
    return {"UserLeft", e.room,
        "{\"room\":" + q(e.room) + ",\"nick\":" + q(e.nick) +
        ",\"timestamp_ms\":" + std::to_string(e.timestamp_ms) + "}"};
}