#pragma once

#include "imessage.hpp"
#include "user.hpp"
#include "chat_errors.hpp"
#include "log.hpp"
#include "concepts.hpp"
#include "message_id.hpp"

#include <map>
#include <vector>
#include <string>
#include <optional>
#include <functional>
#include <memory>
#include <algorithm>
#include <ranges>
#include <iostream>   

class ChatStore {
public:
    ChatStore() = default;

    ChatStore(const ChatStore&) = delete;
    ChatStore& operator=(const ChatStore&) = delete;

    ChatStore(ChatStore&&) noexcept = default;
    ChatStore& operator=(ChatStore&&) noexcept = default;

    
    template<NickLike Nick>
    void create_room(const Nick& room_name) {
        std::string name(room_name);
        if (rooms_.find(name) != rooms_.end()) {
            throw ChatError("Room already exists: '" + name + "'");
        }
        rooms_[name] = Room{};
        if (log_) log_->append("Room created: " + name);
    }

    bool room_exists(const std::string& room_name) const {
        return rooms_.find(room_name) != rooms_.end();
    }

    void post(const std::string& room_name, std::unique_ptr<IMessage> msg) {
        auto it = rooms_.find(room_name);
        if (it == rooms_.end()) throw UnknownRoomError(room_name);
        if (log_) log_->append("[" + room_name + "] " + msg->author() + ": " + msg->render());
        it->second.messages.push_back(std::move(msg));
    }

    void multi_post(const std::vector<std::string>& room_names,
                    std::unique_ptr<IMessage> msg) {
        for (const auto& room : room_names)
            if (!room_exists(room)) throw UnknownRoomError(room);

        std::vector<std::string> succeeded;
        try {
            for (const auto& room : room_names) {
                auto clone = clone_message(*msg);
                rooms_[room].messages.push_back(std::move(clone));
                succeeded.push_back(room);
            }
        } catch (...) {
            for (const auto& room : succeeded) rooms_[room].messages.pop_back();
            throw;
        }
    }

    void react(const std::string& room_name, size_t index, const std::string& emoji) {
        auto it = rooms_.find(room_name);
        if (it == rooms_.end()) throw UnknownRoomError(room_name);
        auto& messages = it->second.messages;
        if (index >= messages.size()) throw ChatError("Message index out of range");
        auto& msg = messages[index];
        if (!msg->isReaction())
            throw TypeMismatchError("ReactionMessage", msg->typeName());
        msg->incrementReaction();
        (void)emoji;  
    }

    std::vector<std::string> history_with_filters(
        const std::string& room_name,
        const std::string& author = "",
        size_t max_count = 0) const
    {
        auto it = rooms_.find(room_name);
        if (it == rooms_.end()) throw UnknownRoomError(room_name);
        const auto& messages = it->second.messages;

        auto filtered = messages | std::views::filter([&](const auto& msg) {
            std::cout << "[DEBUG] filter called for: " << msg->author() << "\n";
            return author.empty() || msg->author() == author;
        });
        auto transformed = filtered | std::views::transform([](const auto& msg) {
            return msg->render();
        });

        std::vector<std::string> result;
        if (max_count == 0) {
            for (auto&& s : transformed) result.emplace_back(std::string(s));
        } else {
            for (auto&& s : transformed | std::views::take(max_count))
                result.emplace_back(std::string(s));
        }
        return result;
    }


    std::vector<std::string> sorted_participants_by_nickname_length(
        const std::string& room_name) const
    {
        auto it = rooms_.find(room_name);
        if (it == rooms_.end()) throw UnknownRoomError(room_name);

        std::vector<std::string> participants;
        for (const auto& wp : it->second.participants)
            if (auto user = wp.lock())
                participants.push_back(user->nickname());

        std::ranges::sort(participants, std::less{},
            [](const std::string& s) { return s.size(); });
        return participants;
    }

    void add_participant(const std::string& room_name, UserPtr user) {
        auto it = rooms_.find(room_name);
        if (it == rooms_.end()) throw UnknownRoomError(room_name);
        it->second.participants.push_back(user);
    }

    void cleanup_participants(const std::string& room_name) {
        auto it = rooms_.find(room_name);
        if (it == rooms_.end()) return;
        auto& p = it->second.participants;
        p.erase(std::remove_if(p.begin(), p.end(),
                [](const UserWeakPtr& wp) { return wp.expired(); }), p.end());
    }

    size_t active_participants(const std::string& room_name) const {
        auto it = rooms_.find(room_name);
        if (it == rooms_.end()) return 0;
        size_t count = 0;
        for (const auto& wp : it->second.participants)
            if (!wp.expired()) ++count;
        return count;
    }

    size_t room_count() const { return rooms_.size(); }

    template<typename Func>
    void for_each_room(Func func) const {
        for (const auto& [name, room] : rooms_) func(name, room.messages);
    }

    void set_log(std::unique_ptr<AppendOnlyLog> log) { log_ = std::move(log); }

    friend void swap(ChatStore& a, ChatStore& b) noexcept {
        using std::swap;
        swap(a.rooms_, b.rooms_);
        swap(a.log_, b.log_);
    }

private:
    struct Room {
        std::vector<std::unique_ptr<IMessage>> messages;
        std::vector<UserWeakPtr> participants;
    };

    static std::unique_ptr<IMessage> clone_message(const IMessage& src) {
        if (auto* t = dynamic_cast<const TextMessage*>(&src))
            return make_text(t->author(), t->render());
        if (auto* s = dynamic_cast<const SystemMessage*>(&src))
            return make_system(s->render());
        if (auto* r = dynamic_cast<const ReactionMessage*>(&src))
            return make_reaction(r->author(), r->emoji(), r->count());
        throw ChatError("Unknown message type for cloning");
    }

    std::map<std::string, Room> rooms_;
    std::unique_ptr<AppendOnlyLog> log_;
};

inline auto dangling_range_demo() {
    std::vector<int> temp = {1, 2, 3, 4, 5};
    return temp | std::views::filter([](int x) { return x > 2; });
}