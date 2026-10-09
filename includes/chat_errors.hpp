#pragma once

#include <stdexcept>
#include <string>
#include <vector>

class ChatError : public std::runtime_error {
public:
    explicit ChatError(const std::string& msg) : std::runtime_error(msg) {}
};

class ParseError : public ChatError {
public:
    explicit ParseError(const std::string& input)
        : ChatError("Parse error: '" + input + "'") {}
};

class UnknownRoomError : public ChatError {
public:
    explicit UnknownRoomError(const std::string& room)
        : ChatError("Unknown room: '" + room + "'"), room_(room) {}
    
    const std::string& room() const { return room_; }

private:
    std::string room_;
};

class NicknameTakenError : public ChatError {
public:
    explicit NicknameTakenError(const std::string& nick)
        : ChatError("Nickname taken: '" + nick + "'"), nick_(nick) {}
    
    const std::string& nick() const { return nick_; }

private:
    std::string nick_;
};

class TypeMismatchError : public ChatError {
public:
    TypeMismatchError(const std::string& expected, const std::string& actual)
        : ChatError("Type mismatch: expected '" + expected + "', got '" + actual + "'")
        , expected_(expected)
        , actual_(actual) {}
    
    const std::string& expected() const { return expected_; }
    const std::string& actual() const { return actual_; }

private:
    std::string expected_;
    std::string actual_;
};