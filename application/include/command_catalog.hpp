#pragma once
#include "ichat_store.hpp"
#include "message_factory.hpp"
#include "ports/presenter.hpp"
#include "ports/event_publisher.hpp"
#include "ports/clock.hpp"
#include "event_serializer.hpp"
#include "command_registry.hpp"
#include <string>

class CommandCatalog {
public:
    CommandCatalog(IChatStore& store, MessageFactory& factory, IPresenter& ui,
                   IEventPublisher& events, IClock& clock)
        : store_(store), factory_(factory), ui_(ui), events_(events), clock_(clock) {}

    void install(CommandRegistry& reg) {
        reg.register_command("/join", [this](RoomName r) {
            if (!store_.room_exists(r.str())) store_.create_room(r.str());
            events_.publish(to_envelope(UserJoined{r.str(), nick_, to_ms(clock_.now())}));
            ui_.success("Комната '" + r.str() + "' создана, вы вошли");
        });
        reg.register_command("/leave", [this](RoomName r) {
            if (!store_.room_exists(r.str())) throw UnknownRoomError(r.str());
            events_.publish(to_envelope(UserLeft{r.str(), nick_, to_ms(clock_.now())}));
            ui_.warn("Вы покинули '" + r.str() + "'");
        });
        reg.register_command("/msg", [this](RoomName r, std::string text) {
            auto m = factory_.text(nick_, text);
            MessagePosted ev{r.str(), m.author(), m.render(), m.id().sequence(), to_ms(m.id().timestamp())};
            store_.post(r.str(), std::move(m));     
            events_.publish(to_envelope(ev));        
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
        reg.register_command("/who",   [this]() { ui_.room_list(store_.room_names()); });
        reg.register_command("/nick",  [this](std::string n) {
            nick_ = n;
            ui_.success("Ник изменён на " + n);
        });
        reg.register_command("/kick", [this](RoomName r, std::string n) {
            events_.publish(to_envelope(UserLeft{r.str(), n, to_ms(clock_.now())}));
            ui_.warn(n + " выгнан из " + r.str());
        });
    }

private:
    IChatStore& store_;
    MessageFactory& factory_;
    IPresenter& ui_;
    IEventPublisher& events_;
    IClock& clock_;
    std::string nick_ = "Никита";
};