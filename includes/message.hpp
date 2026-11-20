#pragma once
#include "message_id.hpp"
#include "message_body.hpp"
#include <string>
#include <memory>
#include <utility>

// Композиция: автор и время — значение-член, а не базовый класс.
struct MessageHeader {
    std::string author;
    MessageId id;
};

class Message {
public:
    Message(MessageHeader header, std::unique_ptr<IMessageBody> body)
        : header_(std::move(header)), body_(std::move(body)) {}

    const std::string& author() const { return header_.author; }
    const MessageId& id() const { return header_.id; }
    std::string render() const { return body_->render(); }
    std::string kind() const { return body_->kind(); }
    void react(const std::string& emoji) { body_->react(emoji); }
    Message clone() const { return Message(header_, body_->clone()); }

    Message(const Message&) = delete;
    Message& operator=(const Message&) = delete;
    Message(Message&&) noexcept = default;
    Message& operator=(Message&&) noexcept = default;
private:
    MessageHeader header_;
    std::unique_ptr<IMessageBody> body_;
};