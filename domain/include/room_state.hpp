#pragma once
#include "message.hpp"
#include "ring_buffer.hpp"
#include "history_query.hpp"
#include "user.hpp"
#include "chat_errors.hpp"
#include <vector>
#include <string>
#include <cstddef>

class RoomState {
public:
    explicit RoomState(size_t capacity);

    RoomState(const RoomState&) = delete;
    RoomState& operator=(const RoomState&) = delete;
    RoomState(RoomState&&) noexcept = default;
    RoomState& operator=(RoomState&&) noexcept = default;

    void post(Message m);
    void rollback();
    size_t message_count() const;
    void react(size_t index, const std::string& emoji);
    std::vector<Message> history(const HistoryQuery& q) const;

    void add_participant(UserPtr u);
    void cleanup_participants();
    size_t active_participants() const;
    std::vector<std::string> nicks_by_length() const;

private:
    RingBuffer<Message> messages_;
    std::vector<UserWeakPtr> participants_;
};