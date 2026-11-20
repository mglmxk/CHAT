#include "chat_app.hpp"
#include "lexer.hpp"
#include "chat_errors.hpp"
#include <vector>
#include <string_view>
#include <cstddef>

ChatApp::ChatApp(IChatStore& store, MessageFactory& factory, IPresenter& ui,
                 IEventPublisher& events, IClock& clock)
    : ui_(&ui) {
    catalog_ = std::make_unique<CommandCatalog>(store, factory, ui, events, clock);
    catalog_->install(registry_);
}

void ChatApp::execute(const std::string& line) {
    auto tokens = Lexer::tokenize(line);
    if (tokens.empty()) return;
    std::string name(tokens[0]);
    std::vector<std::string_view> args(tokens.begin() + 1, tokens.end());

    if (name == "/msg" && args.size() >= 3) {
        size_t offset = static_cast<size_t>(tokens[1].data() + tokens[1].size() - line.data());
        std::string_view text(line.data() + offset, line.size() - offset);
        while (!text.empty() && text.front() == ' ') text.remove_prefix(1);
        args = { tokens[1], text };
    }

    try {
        registry_.execute(name, args);
    } catch (const std::exception& e) {
        ui_->error(e.what());
    }
}