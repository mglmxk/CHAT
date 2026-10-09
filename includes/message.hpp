#pragma once

#include <string>
#include <chrono>
#include <variant>
#include <type_traits>

struct TextBody {
    std::string text;
};

struct SystemNotice {
    std::string text;
};

struct Reaction {
    std::string emoji;
    int count = 0;
};

using MessageBody = std::variant<TextBody, SystemNotice, Reaction>;

class Message {
public:
    Message(std::string author, MessageBody body)
        : author_(std::move(author))
        , body_(std::move(body))
        , timestamp_(std::chrono::system_clock::now()) {}

    const std::string& author() const { return author_; }

    const MessageBody& body() const { return body_; }
    
    MessageBody& body() { return body_; }
    
    auto timestamp() const { return timestamp_; }

    std::string typeName() const {
        return std::visit([](const auto& body) -> std::string {
            using T = std::decay_t<decltype(body)>;
            if constexpr (std::is_same_v<T, TextBody>) {
                return "TextBody";
            } else if constexpr (std::is_same_v<T, SystemNotice>) {
                return "SystemNotice";
            } else if constexpr (std::is_same_v<T, Reaction>) {
                return "Reaction";
            }
            return "Unknown";
        }, body_);
    }

private:
    std::string author_;
    MessageBody body_;
    std::chrono::system_clock::time_point timestamp_;
};