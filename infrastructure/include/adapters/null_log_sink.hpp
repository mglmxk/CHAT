#pragma once
#include "ports/log_sink.hpp"

class NullLogSink final : public ILogSink {
public:
    void append(std::string_view) override {}
};