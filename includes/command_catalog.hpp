#pragma once
#include "ichat_store.hpp"
#include "message_factory.hpp"
#include "ports/presenter.hpp"
#include "command_registry.hpp"
#include <string>

class CommandCatalog {
public:
    CommandCatalog(IChatStore& store, MessageFactory& factory, IPresenter& ui)
        : store_(store), factory_(factory), ui_(ui) {}

    void install(CommandRegistry& reg) {
        reg.register_command("/join", [this](RoomName r) {
            store_.create_room(r.str());
            ui_.success("Комната '" + r.str() + "' создана");
        });
        reg.register_command("/leave", [this](RoomName r) {
            if (!store_.room_exists(r.str())) throw UnknownRoomError(r.str());
            ui_.warn("Вы покинули '" + r.str() + "'");
        });
        reg.register_command("/msg", [this](RoomName r, std::string text) {
            store_.post(r.str(), factory_.text("Никита", std::move(text)));
            ui_.success("Отправлено в '" + r.str() + "'");
        });
        reg.register_command("/topic", [this](RoomName r, std::string topic) {
            store_.post(r.str(), factory_.system("Тема: " + topic));
            ui_.success("Тема '" + r.str() + "' изменена");
        });
        reg.register_command("/react", [this](RoomName r, size_t i, std::string emoji) {
            store_.react(r.str(), i, emoji);
            ui_.success("Реакция " + emoji + " добавлена");
        });
        reg.register_command("/history", [this](RoomName r) {
            ui_.history(r.str(), store_.history(r.str(), HistoryQuery{}));
        });
        reg.register_command("/history-by", [this](RoomName r, std::string author) {
            ui_.history(r.str(), store_.history(r.str(), HistoryQuery{author, 0}));
        });
        reg.register_command("/rooms", [this]() { ui_.room_list(store_.room_names()); });
        reg.register_command("/who", [this]() { ui_.room_list(store_.room_names()); });
        reg.register_command("/nick", [this](std::string n) { ui_.success("Ник изменён на " + n); });
        reg.register_command("/kick", [this](RoomName r, std::string n) {
            ui_.warn(n + " выгнан из " + r.str());
        });
    }

private:
    IChatStore& store_;
    MessageFactory& factory_;
    IPresenter& ui_;
};