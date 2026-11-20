#pragma once
#include "ichat_store.hpp"
#include "ports/log_sink.hpp"
#include "room_state.hpp"
#include "chat_errors.hpp"
#include <map>
#include <algorithm>

class StdEngine final : public IChatStore {
public:
    explicit StdEngine(ILogSink& sink, size_t capacity = 100)
        : sink_(sink), capacity_(capacity) {}

    void create_room(const std::string& name) override {
        if (rooms_.contains(name)) throw ChatError("Room already exists: '" + name + "'");
        rooms_.emplace(name, RoomState(capacity_));
        sink_.append("room created: " + name);
    }
    bool room_exists(const std::string& name) const override { return rooms_.contains(name); }
    size_t room_count() const override { return rooms_.size(); }

    std::vector<std::string> room_names() const override {
        std::vector<std::string> out;
        out.reserve(rooms_.size());
        std::transform(rooms_.begin(), rooms_.end(), std::back_inserter(out),
                       [](const auto& kv) { return kv.first; });
        return out;  // std::map уже даёт алфавитный порядок
    }

    void post(const std::string& room, Message msg) override {
        auto& r = lookup(room);
        sink_.append("[" + room + "] " + msg.author() + ": " + msg.render());
        r.post(std::move(msg));
    }

    void multi_post(const std::vector<std::string>& rooms, const Message& msg) override {
        for (const auto& n : rooms) if (!room_exists(n)) throw UnknownRoomError(n);
        std::vector<std::string> done;
        try {
            for (const auto& n : rooms) {
                lookup(n).post(msg.clone());
                done.push_back(n);
            }
        } catch (...) {
            for (const auto& n : done) lookup(n).rollback();
            throw;
        }
    }

    void react(const std::string& room, size_t i, const std::string& e) override { lookup(room).react(i, e); }
    std::vector<Message> history(const std::string& room, const HistoryQuery& q) const override {
        return lookup(room).history(q);
    }
    void add_participant(const std::string& room, UserPtr u) override { lookup(room).add_participant(std::move(u)); }
    void cleanup_participants(const std::string& room) override { lookup(room).cleanup_participants(); }
    size_t active_participants(const std::string& room) const override { return lookup(room).active_participants(); }
    std::vector<std::string> nicks_by_length(const std::string& room) const override { return lookup(room).nicks_by_length(); }

    void for_each_room(const std::function<void(const std::string&, size_t&)>& fn) const override {
        for (const auto& [name, room] : rooms_) {
            size_t n = room.message_count();
            fn(name, n);
        }
    }

private:
    RoomState& lookup(const std::string& n) {
        auto it = rooms_.find(n);
        if (it == rooms_.end()) throw UnknownRoomError(n);
        return it->second;
    }
    const RoomState& lookup(const std::string& n) const {
        auto it = rooms_.find(n);
        if (it == rooms_.end()) throw UnknownRoomError(n);
        return it->second;
    }

    ILogSink& sink_;
    size_t capacity_;
    std::map<std::string, RoomState> rooms_;
};