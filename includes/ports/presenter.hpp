#pragma once
#include "message.hpp"
#include <string_view>
#include <vector>

class IPresenter {
public:
    virtual ~IPresenter() = default;
    virtual void success(std::string_view text) = 0;
    virtual void info(std::string_view text) = 0;
    virtual void warn(std::string_view text) = 0;
    virtual void error(std::string_view text) = 0;
    virtual void room_list(const std::vector<std::string>& names) = 0;
    virtual void history(std::string_view room, const std::vector<Message>& msgs) = 0;
};