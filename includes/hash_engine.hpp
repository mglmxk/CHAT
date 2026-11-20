#pragma once
#include "ichat_store.hpp"
#include "ports/log_sink.hpp"
#include "room_state.hpp"
#include "hash_table.hpp"
#include "fnv_hash.hpp"
#include "chat_errors.hpp"
#include <algorithm>
#include <ranges>

class HashEngine final : public IChatStore {
public:
    explicit HashEngine(ILogSink& sink, size_t capacity = 100)
        : sink_(sink), capacity_(capacity), rooms_(16) {}

    void create_room(const std::string& name) override {
        if (rooms_.contains(name)) throw ChatError("Room already exists: '" + name + "'");
        rooms_.insert(name, RoomState(capacity_));
        sink_.append("room created: " + name);
    }
    bool room_exists(const std::string& name) const override { return rooms_.contains(name); }
    size_t room_count() const override { return rooms_.size(); }

    std::vector<std::string> room_names() const override {
        std::vector<std::string> out;
        out.reserve(rooms_.size());
        for (auto it = rooms_.begin(); it != rooms_.end(); ++it) out.push_back(it->first);
        std::ranges::sort(out);  // хеш-таблица порядка не даёт — сортируем явно
        return out;
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
            for (const auto& n : rooms) { lookup(n).post(msg.clone()); done.push_back(n); }
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
        for (auto it = rooms_.begin(); it != rooms_.end(); ++it) {
            size_t n = it->second.message_count();
            fn(it->first, n);
        }
    }

private:
    RoomState& lookup(const std::string& n) {
        if (!rooms_.contains(n)) throw UnknownRoomError(n);
        return rooms_.at(n);
    }
    const RoomState& lookup(const std::string& n) const {
        if (!rooms_.contains(n)) throw UnknownRoomError(n);
        return rooms_.at(n);
    }

    ILogSink& sink_;
    size_t capacity_;
    HashTable<std::string, RoomState, Fnv1aHash> rooms_;
};