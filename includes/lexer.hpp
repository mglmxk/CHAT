#pragma once

#include <string_view>
#include <vector>

class Lexer {
public:
    static std::vector<std::string_view> tokenize(std::string_view input) {
        std::vector<std::string_view> tokens;
        size_t pos = 0;
        size_t len = input.size();

        while (pos < len && isSpace(input[pos])) ++pos;

        while (pos < len) {
            if (input[pos] == '"') {
                size_t start = pos + 1;
                size_t end = input.find('"', start);
                if (end == std::string_view::npos) {
                    end = len;
                }
                tokens.push_back(input.substr(start, end - start));
                pos = end + 1;
            } else {
                size_t start = pos;
                while (pos < len && !isSpace(input[pos])) ++pos;
                tokens.push_back(input.substr(start, pos - start));
            }

            while (pos < len && isSpace(input[pos])) ++pos;
        }

        return tokens;
    }

    static std::vector<std::string_view> tokenizeWithRest(std::string_view input) {
        std::vector<std::string_view> tokens = tokenize(input);
        
        if (tokens.size() >= 3 && tokens[0] == "/msg") {
            size_t restStart = input.find(tokens[2]);
            if (restStart != std::string_view::npos) {
                std::string_view rest = input.substr(restStart);
                while (!rest.empty() && isSpace(rest.front())) {
                    rest = rest.substr(1);
                }
                tokens.resize(2);
                tokens.push_back(rest);
            }
        }

        return tokens;
    }

private:
    static bool isSpace(char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }
};