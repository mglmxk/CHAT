#pragma once
#include "ichat_store.hpp"
#include "ports/presenter.hpp"
#include "command_registry.hpp"
#include "command_catalog.hpp"
#include "lexer.hpp"
#include "chat_errors.hpp"
#include <string>
#include <vector>
#include <string_view>

// GRASP Controller: только маршрутизация ввода, никакой бизнес-логики и вывода.
class ChatApp {
public:
    ChatApp(IChatStore& store, MessageFactory& factory, IPresenter& ui) {
        catalog_ = std::make_unique<CommandCatalog>(store, factory, ui);
        catalog_->install(registry_);
        ui_ = &ui;
    }

    void execute(const std::string& line) {
        auto tokens = Lexer::tokenize(line);
        if (tokens.empty()) return;
        std::string name(tokens[0]);
        std::vector<std::string_view> args(tokens.begin() + 1, tokens.end());
        try {
            registry_.execute(name, args);
        } catch (const std::exception& e) {
            ui_->error(e.what());
        }
    }

private:
    CommandRegistry registry_;
    std::unique_ptr<CommandCatalog> catalog_;
    IPresenter* ui_ = nullptr;
};