#pragma once

#include "function_traits.hpp"
#include "chat_errors.hpp"
#include <string>
#include <string_view>
#include <map>
#include <functional>
#include <charconv>
#include <concepts>
#include <type_traits>

struct RoomName {
    std::string value;
    explicit RoomName(std::string v) : value(std::move(v)) {
        if (value.empty() || value[0] != '#') {
            throw ParseError("Room name must start with '#': " + value);
        }
    }
    const std::string& str() const { return value; }
};

template<typename T>
T convert(std::string_view input) {
    if constexpr (std::is_same_v<T, std::string>) {
        return std::string(input);
    }
    else if constexpr (std::is_same_v<T, std::string_view>) {
        return input;
    }
    else if constexpr (std::is_same_v<T, RoomName>) {
        return RoomName(std::string(input));
    }
    else if constexpr (std::is_arithmetic_v<T>) {
        T result{};
        auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), result);
        if (ec != std::errc{}) {
            throw ParseError("Cannot convert '" + std::string(input) + "' to number");
        }
        return result;
    }
    else {
        static_assert(sizeof(T) == 0, 
            "Unsupported type for convert<T>. Supported: string, string_view, RoomName, arithmetic types.");
    }
}

template<typename T, std::enable_if_t<std::is_same_v<T, std::string>, int> = 0>
T convert_enable_if(std::string_view input) { return std::string(input); }

template<typename T, std::enable_if_t<std::is_same_v<T, std::string_view>, int> = 0>
T convert_enable_if(std::string_view input) { return input; }

template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
T convert_enable_if(std::string_view input) {
    T result{};
    auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), result);
    if (ec != std::errc{}) throw ParseError("Cannot convert to number");
    return result;
}

template<typename T, std::enable_if_t<std::is_same_v<T, RoomName>, int> = 0>
T convert_enable_if(std::string_view input) { return RoomName(std::string(input)); }

template<typename T>
concept CommandHandler = requires {
    typename function_traits<T>;
};

class CommandRegistry {
public:
    using Handler = std::function<void(std::vector<std::string_view>)>;
    
    template<CommandHandler F>
    void register_command(const std::string& name, F handler) {
        using traits = function_traits<F>;
        constexpr size_t arity = traits::arity;
    
        handlers_[name] = [handler, arity](std::vector<std::string_view> tokens) {
            if (tokens.size() != arity) {
                throw ParseError("Command expects " + std::to_string(arity) + 
                               " arguments, got " + std::to_string(tokens.size()));
            }
            call_with_converted_args<traits>(handler, tokens, std::make_index_sequence<arity>{});
        };
    }
    
    void execute(const std::string& name, std::vector<std::string_view> tokens) const {
        auto it = handlers_.find(name);
        if (it == handlers_.end()) {
            throw ParseError("Unknown command: " + name);
        }
        it->second(std::move(tokens));
    }

private:
    template<typename Traits, typename F, size_t... Is>
    static void call_with_converted_args(F& handler, 
                                         std::vector<std::string_view>& tokens,
                                         std::index_sequence<Is...>) {
        handler(convert<typename Traits::template arg_type<Is>>(tokens[Is])...);
    }
    
    std::map<std::string, Handler> handlers_;
};