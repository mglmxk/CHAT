#pragma once
#include "message.hpp"
#include "history_query.hpp"
#include "user.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>

class IChatStore {
public:
    virtual ~IChatStore() = default;

    virtual void create_room(const std::string& name) = 0;
    virtual bool room_exists(const std::string& name) const = 0;
    virtual std::vector<std::string> room_names() const = 0;
    virtual size_t room_count() const = 0;

    virtual void post(const std::string& room, Message msg) = 0;
    virtual void multi_post(const std::vector<std::string>& rooms, const Message& msg) = 0;
    virtual void react(const std::string& room, size_t index, const std::string& emoji) = 0;
    virtual std::vector<Message> history(const std::string& room, const HistoryQuery& q) const = 0;

    virtual void add_participant(const std::string& room, UserPtr user) = 0;
    virtual void cleanup_participants(const std::string& room) = 0;
    virtual size_t active_participants(const std::string& room) const = 0;
    virtual std::vector<std::string> nicks_by_length(const std::string& room) const = 0;

    virtual void for_each_room(const std::function<void(const std::string&, size_t&)>& fn) const = 0;
};