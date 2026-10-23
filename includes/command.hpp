#pragma once

#include <string>
#include <variant>
#include <optional>
#include <vector>
#include "lexer.hpp"
#include "chat_errors.hpp"

struct JoinCommand {
    std::string room;
};

struct LeaveCommand {
    std::string room;
};

struct MsgCommand {
    std::string room;
    std::string text;
};

struct MultiMsgCommand {
    std::vector<std::string> rooms;
    std::string text;
};

struct WhoCommand {};

struct HistoryCommand {
    std::string room;
};

struct TopicCommand {
    std::string room;
    std::string topic;
};

struct ReactCommand {
    std::string room;
    int index;
    std::string emoji;
};

using Command = std::variant<
    JoinCommand,
    LeaveCommand,
    MsgCommand,
    MultiMsgCommand,
    WhoCommand,
    HistoryCommand,
    TopicCommand,
    ReactCommand
>;

class CommandParser {
public:
    static std::optional<Command> parse(std::string_view input) {
        auto tokens = Lexer::tokenizeWithRest(input);
        
        if (tokens.empty()) {
            return std::nullopt;
        }

        std::string_view cmd = tokens[0];

        if (cmd == "/join" && tokens.size() >= 2) {
            return JoinCommand{std::string(tokens[1])};
        }
        if (cmd == "/leave" && tokens.size() >= 2) {
            return LeaveCommand{std::string(tokens[1])};
        }
        if (cmd == "/msg") {
        if (tokens.size() < 3) {
        throw ParseError(std::string(input));
    }
    
    if (tokens.size() > 3) {
        MultiMsgCommand multi;  
        for (size_t i = 1; i < tokens.size() - 1; ++i) {
            multi.rooms.push_back(std::string(tokens[i]));
        }
        multi.text = std::string(tokens.back());
        return multi;
    }
    return MsgCommand{std::string(tokens[1]), std::string(tokens[2])};
}
        if (cmd == "/who") {
            return WhoCommand{};
        }
        if (cmd == "/history" && tokens.size() >= 2) {
            return HistoryCommand{std::string(tokens[1])};
        }
        if (cmd == "/topic" && tokens.size() >= 3) {
            return TopicCommand{std::string(tokens[1]), std::string(tokens[2])};
        }
        if (cmd == "/react" && tokens.size() >= 4) {
            try {
                int idx = std::stoi(std::string(tokens[2]));
                return ReactCommand{
                    std::string(tokens[1]), idx, std::string(tokens[3])
                };
            } catch (const std::exception&) {
                throw ParseError(std::string(input));
            }
        }

        throw ParseError(std::string(input));
    }
};

template<typename... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template<typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;