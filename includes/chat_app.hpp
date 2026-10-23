#pragma once

#include "chat_store.hpp"
#include "command.hpp"
#include "chat_errors.hpp"
#include <iostream>
#include <string>

class ChatApp {
public:
    void execute(const Command& cmd) {
        try {
            std::visit(overloaded{
                [this](const JoinCommand& c) -> void {
                    store_.create_room(c.room);
                    std::cout << "Комната '" << c.room << "' создана.\n";
                },
                [this](const LeaveCommand& c) -> void {
                    if (!store_.room_exists(c.room)) {
                        throw UnknownRoomError(c.room);
                    }
                    std::cout << "Вы покинули комнату '" << c.room << "'.\n";
                },
                [this](const MsgCommand& c) -> void {
                    store_.post(c.room, make_text("User", c.text));
                    std::cout << "Сообщение отправлено в '" << c.room << "'.\n";
                },
                [this](const MultiMsgCommand& c) -> void {
                    auto msg = make_text("User", c.text);
                    store_.multi_post(c.rooms, std::move(msg));
                    std::cout << "Сообщение разослано в " << c.rooms.size() 
                              << " комнат.\n";
                },
                [this](const WhoCommand&) -> void {
                    std::cout << "Список комнат:\n";
                    store_.for_each_room([](const std::string& name, const auto& msgs) {
                        std::cout << "  - " << name << " (" << msgs.size() << " msg)\n";
                    });
                },
               [this](const HistoryCommand& c) -> void {
   
                auto history = store_.history_with_filters(c.room);
    
                 std::cout << "История '" << c.room << "':\n";
                if (history.empty()) {
                std::cout << "  (пусто)\n";
                 } else {
                    size_t i = 0;
                    for (const auto& msg : history) {
                    std::cout << "  [" << i++ << "] " << msg << "\n";
                 }
                    }
                },
                [this](const TopicCommand& c) -> void {
                    if (!store_.room_exists(c.room)) {
                        throw UnknownRoomError(c.room);
                    }
                    store_.post(c.room, make_system("Тема изменена на: " + c.topic));
                    std::cout << "Тема комнаты '" << c.room 
                              << "' изменена на: " << c.topic << "\n";
                },
                [this](const ReactCommand& c) -> void {
                    store_.react(c.room, c.index, c.emoji);
                    std::cout << "Реакция '" << c.emoji << "' добавлена.\n";
                }
            }, cmd);
        }
        catch (const ChatError& e) {
            std::cout << "[ОШИБКА] " << e.what() << "\n";
        }
        catch (const std::exception& e) {
            std::cout << "[ОШИБКА] " << e.what() << "\n";
        }
    }

    ChatStore& store() { return store_; }

private:
    ChatStore store_;
};