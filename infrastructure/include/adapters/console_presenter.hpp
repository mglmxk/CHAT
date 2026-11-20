#pragma once
#include "ports/presenter.hpp"
#include "colors.hpp"
#include <iostream>

class ConsolePresenter final : public IPresenter {
public:
    void success(std::string_view t) override { std::cout << Colors::GREEN << "✅ " << t << "\n" << Colors::RESET; }
    void info(std::string_view t) override    { std::cout << Colors::CYAN << t << "\n" << Colors::RESET; }
    void warn(std::string_view t) override    { std::cout << Colors::YELLOW << t << "\n" << Colors::RESET; }
    void error(std::string_view t) override   { std::cout << Colors::RED << "❌ " << t << "\n" << Colors::RESET; }

    void room_list(const std::vector<std::string>& names) override {
        std::cout << Colors::CYAN << Colors::BOLD << "\n🏠 Комнаты:\n" << Colors::RESET;
        if (names.empty()) std::cout << "  (нет комнат)\n";
        for (const auto& n : names)
            std::cout << "  " << Colors::YELLOW << "▸ " << n << Colors::RESET << "\n";
        std::cout << "\n";
    }

    void history(std::string_view room, const std::vector<Message>& msgs) override {
        std::cout << Colors::CYAN << Colors::BOLD << "\n📜 История " << room << ":\n" << Colors::RESET;
        if (msgs.empty()) std::cout << "  (пусто)\n";
        size_t i = 0;
        for (const auto& m : msgs)
            std::cout << "  [" << i++ << "] " << m.author() << ": " << m.render() << "\n";
        std::cout << "\n";
    }
};