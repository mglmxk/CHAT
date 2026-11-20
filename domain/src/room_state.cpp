#include "room_state.hpp"
#include <algorithm>
#include <ranges>
#include <iterator>
#include <functional>
#include <utility>

RoomState::RoomState(size_t capacity) : messages_(capacity) {}

void RoomState::post(Message m) { messages_.push_back(std::move(m)); }
void RoomState::rollback() { messages_.rollback(); }
size_t RoomState::message_count() const { return messages_.size(); }

void RoomState::react(size_t index, const std::string& emoji) {
    if (index >= messages_.size()) throw ChatError("Message index out of range");
    messages_[index].react(emoji);
}

std::vector<Message> RoomState::history(const HistoryQuery& q) const {
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

void RoomState::add_participant(UserPtr u) { participants_.push_back(std::move(u)); }

void RoomState::cleanup_participants() {
    std::erase_if(participants_, [](const UserWeakPtr& w) { return w.expired(); });
}

size_t RoomState::active_participants() const {
    return static_cast<size_t>(std::count_if(participants_.begin(), participants_.end(),
                   [](const UserWeakPtr& w) { return !w.expired(); }));
}

std::vector<std::string> RoomState::nicks_by_length() const {
    std::vector<std::string> nicks;
    for (const auto& w : participants_)
        if (auto u = w.lock()) nicks.push_back(u->nickname());
    std::ranges::sort(nicks, std::less{}, [](const std::string& s) { return s.size(); });
    return nicks;
}