#pragma once

#include <string>
#include <memory>
#include <chrono>

class IMessage {
public:
    virtual ~IMessage() = default;

    virtual const std::string& author() const = 0;
    virtual std::string typeName() const = 0;
    virtual std::string render() const = 0;
    virtual bool isReaction() const { return false; }
    virtual void incrementReaction() {}

    auto timestamp() const { return timestamp_; }

protected:
    std::chrono::system_clock::time_point timestamp_ = 
        std::chrono::system_clock::now();
};

class TextMessage : public IMessage {
public:
    TextMessage(std::string author, std::string text)
        : author_(std::move(author)), text_(std::move(text)) {}

    const std::string& author() const override { return author_; }
    std::string typeName() const override { return "TextMessage"; }
    std::string render() const override { return text_; }

private:
    std::string author_;
    std::string text_;
};

class SystemMessage : public IMessage {
public:
    SystemMessage(std::string text) : text_(std::move(text)) {}

    const std::string& author() const override {
        static const std::string system = "SYSTEM";
        return system;
    }
    std::string typeName() const override { return "SystemMessage"; }
    std::string render() const override { return text_; }

private:
    std::string text_;
};

class ReactionMessage : public IMessage {
public:
    ReactionMessage(std::string author, std::string emoji, int count = 0)
        : author_(std::move(author)), emoji_(std::move(emoji)), count_(count) {}

    const std::string& author() const override { return author_; }
    std::string typeName() const override { return "ReactionMessage"; }
    std::string render() const override { 
        return emoji_ + " x" + std::to_string(count_); 
    }
    bool isReaction() const override { return true; }
    void incrementReaction() override { ++count_; }

    const std::string& emoji() const { return emoji_; }
    int count() const { return count_; }

private:
    std::string author_;
    std::string emoji_;
    int count_;
};

inline std::unique_ptr<IMessage> make_text(std::string author, std::string text) {
    return std::make_unique<TextMessage>(std::move(author), std::move(text));
}

inline std::unique_ptr<IMessage> make_system(std::string text) {
    return std::make_unique<SystemMessage>(std::move(text));
}

inline std::unique_ptr<IMessage> make_reaction(std::string author, 
                                                std::string emoji, int count = 0) {
    return std::make_unique<ReactionMessage>(
        std::move(author), std::move(emoji), count);
}