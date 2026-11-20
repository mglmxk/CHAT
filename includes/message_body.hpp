#pragma once
#include "chat_errors.hpp"
#include <string>
#include <memory>
#include <utility>


class IMessageBody {
public:
    virtual ~IMessageBody() = default;
    virtual std::string render() const = 0;
    virtual std::string kind() const = 0;
    virtual std::unique_ptr<IMessageBody> clone() const = 0;

   
    virtual void react(const std::string& emoji) {
        (void)emoji;
        throw TypeMismatchError("ReactionableMessage", kind());
    }
};

class TextBody final : public IMessageBody {
public:
    explicit TextBody(std::string text) : text_(std::move(text)) {}
    std::string render() const override { return text_; }
    std::string kind() const override { return "Text"; }
    std::unique_ptr<IMessageBody> clone() const override { return std::make_unique<TextBody>(text_); }
private:
    std::string text_;
};

class SystemBody final : public IMessageBody {
public:
    explicit SystemBody(std::string text) : text_(std::move(text)) {}
    std::string render() const override { return "[system] " + text_; }
    std::string kind() const override { return "System"; }
    std::unique_ptr<IMessageBody> clone() const override { return std::make_unique<SystemBody>(text_); }
private:
    std::string text_;
};

class ReactionBody final : public IMessageBody {
public:
    explicit ReactionBody(std::string emoji, unsigned count = 1)
        : emoji_(std::move(emoji)), count_(count) {}
    std::string render() const override { return emoji_ + " x" + std::to_string(count_); }
    std::string kind() const override { return "Reaction"; }
    std::unique_ptr<IMessageBody> clone() const override { return std::make_unique<ReactionBody>(emoji_, count_); }
    void react(const std::string&) override { ++count_; }
private:
    std::string emoji_;
    unsigned count_;
};