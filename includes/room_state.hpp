#pragma once
#include "message.hpp"
#include "ring_buffer.hpp"
#include "history_query.hpp"
#include "user.hpp"
#include "chat_errors.hpp"
#include <vector>
#include <algorithm>
#include <ranges>
#include <iterator>
#include <functional>

// Information Expert: все операции над данными комнаты живут здесь,
// а не дублируются в двух движках.
class RoomState {
public:
    explicit RoomState(size_t capacity) : messages_(capacity) {}

    RoomState(const RoomState&) = delete;
    RoomState& operator=(const RoomState&) = delete;
    RoomState(RoomState&&) noexcept = default;
    RoomState& operator=(RoomState&&) noexcept = default;

    void post(Message m) { messages_.push_back(std::move(m)); }
    void rollback() { messages_.rollback(); }
    size_t message_count() const { return messages_.size(); }

    void react(size_t index, const std::string& emoji) {
        if (index >= messages_.size()) throw ChatError("Message index out of range");
        messages_[index].react(emoji);
    }

    std::vector<Message> history(const HistoryQuery& q) const {
        std::vector<const Message*> selected;
        selected.reserve(messages_.size());
        for (size_t i = 0; i < messages_.size(); ++i) selected.push_back(&messages_[i]);

        std::erase_if(selected, [&](const Message* m) {
            return !q.author.empty() && m->author() != q.author;
        });
        if (q.limit != 0 && selected.size() > q.limit) selected.resize(q.limit);

        std::vector<Message> out;
        out.reserve(selected.size());
        std::transform(selected.begin(), selected.end(), std::back_inserter(out),
                       [](const Message* m) { return m->clone(); });
        return out;
    }

    void add_participant(UserPtr u) { participants_.push_back(std::move(u)); }
    void cleanup_participants() {
        std::erase_if(participants_, [](const UserWeakPtr& w) { return w.expired(); });
    }
    size_t active_participants() const {
        return static_cast<size_t>(std::count_if(participants_.begin(), participants_.end(),
                       [](const UserWeakPtr& w) { return !w.expired(); }));
    }
    std::vector<std::string> nicks_by_length() const {
        std::vector<std::string> nicks;
        for (const auto& w : participants_)
            if (auto u = w.lock()) nicks.push_back(u->nickname());
        std::ranges::sort(nicks, std::less{}, [](const std::string& s) { return s.size(); });
        return nicks;
    }

private:
    RingBuffer<Message> messages_;
    std::vector<UserWeakPtr> participants_;
};