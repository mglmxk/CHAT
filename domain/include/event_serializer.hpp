#pragma once
#include "events.hpp"
#include "event_envelope.hpp"

EventEnvelope to_envelope(const MessagePosted& e);
EventEnvelope to_envelope(const UserJoined& e);
EventEnvelope to_envelope(const UserLeft& e);