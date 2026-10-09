#pragma once

#include "imessage.hpp"
#include "user.hpp"
#include "chat_errors.hpp"
#include "log.hpp"
#include <map>
#include <vector>
#include <string>
#include <optional>
#include <functional>
#include <memory>
#include <algorithm>

class ChatStore {
public:
    ChatStore() = default;

    ChatStore(const ChatStore&) = delete;
    ChatStore& operator=(const ChatStore&) = delete;

    ChatStore(ChatStore&&) noexcept = default;
    ChatStore& operator=(ChatStore&&) noexcept = default;

    void create_room(const std::string& room_name) {
        if (rooms_.find(room_name) != rooms_.end()) {
            throw ChatError("Room already exists: '" + room_name + "'");
        }
        rooms_[room_name] = Room{};
        if (log_) {
            log_->append("Room created: " + room_name);
        }
    }

    bool room_exists(const std::string& room_name) const {
        return rooms_.find(room_name) != rooms_.end();
    }

    void post(const std::string& room_name, std::unique_ptr<IMessage> msg) {
        auto it = rooms_.find(room_name);
        if (it == rooms_.end()) {
            throw UnknownRoomError(room_name);
        }
        if (log_) {
            log_->append("[" + room_name + "] " + msg->author() + ": " + msg->render());
        }
        it->second.messages.push_back(std::move(msg));
    }

    void multi_post(const std::vector<std::string>& room_names, 
                    std::unique_ptr<IMessage> msg) {
        for (const auto& room : room_names) {
            if (!room_exists(room)) {
                throw UnknownRoomError(room);
            }
        }

        
        std::vector<std::string> succeeded;
        try {
            for (const auto& room : room_names) {
                // Клонируем сообщение для каждой комнаты
                auto clone = clone_message(*msg);
                rooms_[room].messages.push_back(std::move(clone));
                succeeded.push_back(room);
            }
            if (log_) {
                log_->append("Multi-post to " + std::to_string(room_names.size()) + " rooms");
            }
        } catch (...) {

            for (const auto& room : succeeded) {
                rooms_[room].messages.pop_back();
            }
            throw;
        }
    }

    void react(const std::string& room_name, size_t index, const std::string& emoji) {
        auto it = rooms_.find(room_name);
        if (it == rooms_.end()) {
            throw UnknownRoomError(room_name);
        }

        auto& messages = it->second.messages;
        if (index >= messages.size()) {
            throw ChatError("Message index out of range");
        }

        auto& msg = messages[index];
        if (!msg->isReaction()) {
            throw TypeMismatchError("ReactionMessage", msg->typeName());
        }

        msg->incrementReaction();
    }

    std::optional<std::reference_wrapper<const std::vector<std::unique_ptr<IMessage>>>> 
    history(const std::string& room_name) const {
        auto it = rooms_.find(room_name);
        if (it != rooms_.end()) {
            return std::cref(it->second.messages);
        }
        return std::nullopt;
    }

    void add_participant(const std::string& room_name, UserPtr user) {
        auto it = rooms_.find(room_name);
        if (it == rooms_.end()) {
            throw UnknownRoomError(room_name);
        }
        it->second.participants.push_back(user);
    }

    void cleanup_participants(const std::string& room_name) {
        auto it = rooms_.find(room_name);
        if (it == rooms_.end()) return;

        auto& participants = it->second.participants;
        participants.erase(
            std::remove_if(participants.begin(), participants.end(),
                [](const UserWeakPtr& wp) { return wp.expired(); }),
            participants.end());
    }

    size_t active_participants(const std::string& room_name) const {
        auto it = rooms_.find(room_name);
        if (it == rooms_.end()) return 0;

        size_t count = 0;
        for (const auto& wp : it->second.participants) {
            if (!wp.expired()) ++count;
        }
        return count;
    }

    size_t room_count() const { return rooms_.size(); }

    template<typename Func>
    void for_each_room(Func func) const {
        for (const auto& [name, room] : rooms_) {
            func(name, room.messages);
        }
    }

    void set_log(std::unique_ptr<AppendOnlyLog> log) {
        log_ = std::move(log);
    }

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
        if (auto* t = dynamic_cast<const TextMessage*>(&src)) {
            return make_text(t->author(), t->render());
        }
        if (auto* s = dynamic_cast<const SystemMessage*>(&src)) {
            return make_system(s->render());
        }
        if (auto* r = dynamic_cast<const ReactionMessage*>(&src)) {
            return make_reaction(r->author(), r->emoji(), r->count());
        }
        throw ChatError("Unknown message type for cloning");
    }

    std::map<std::string, Room> rooms_;
    std::unique_ptr<AppendOnlyLog> log_;
};