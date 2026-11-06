#pragma once

#include "chat_store.hpp"
#include "command_registry.hpp"
#include "chat_errors.hpp"
#include "lexer.hpp"
#include <iostream>
#include <string>
#include <vector>

class ChatApp {
public:
    ChatApp() {
        register_all_commands();
    }
    
    void execute(const std::string& input) {
        auto tokens = Lexer::tokenize(input);
        if (tokens.empty()) {
            std::cout << "Пустой ввод.\n";
            return;
        }
        
        std::string cmd_name(tokens[0]);
        std::vector<std::string_view> args(tokens.begin() + 1, tokens.end());
        
        try {
            registry_.execute(cmd_name, args);
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
    void register_all_commands() {
        registry_.register_command("/join", [this](RoomName room) {
            store_.create_room(room.str());
            std::cout << "Комната '" << room.str() << "' создана.\n";
        });

        registry_.register_command("/leave", [this](RoomName room) {
            if (!store_.room_exists(room.str())) throw UnknownRoomError(room.str());
            std::cout << "Вы покинули комнату '" << room.str() << "'.\n";
        });
        
        registry_.register_command("/msg", [this](RoomName room, std::string text) {
            store_.post(room.str(), make_text("User", std::move(text)));
            std::cout << "Сообщение отправлено в '" << room.str() << "'.\n";
        });
        
        registry_.register_command("/who", [this]() {
            std::cout << "Список комнат:\n";
            store_.for_each_room([](const std::string& name, const auto& msgs) {
                std::cout << "  - " << name << " (" << msgs.size() << " msg)\n";
            });
        });
        
        registry_.register_command("/history", [this](RoomName room) {
            auto history = store_.history_with_filters(room.str());
            std::cout << "История '" << room.str() << "':\n";
            if (history.empty()) std::cout << "  (пусто)\n";
            else {
                size_t i = 0;
                for (const auto& msg : history) std::cout << "  [" << i++ << "] " << msg << "\n";
            }
        });
        
        registry_.register_command("/topic", [this](RoomName room, std::string topic) {
            if (!store_.room_exists(room.str())) throw UnknownRoomError(room.str());
            store_.post(room.str(), make_system("Тема изменена на: " + topic));
            std::cout << "Тема комнаты '" << room.str() << "' изменена на: " << topic << "\n";
        });

        registry_.register_command("/type", [this](RoomName room, size_t index) {
            auto h = store_.history_with_filters(room.str());
            if (index >= h.size()) throw ChatError("Index out of range");
            std::cout << "Тип сообщения [" << index << "]: Text\n"; 
        });
        
        registry_.register_command("/react", [this](RoomName room, size_t index, std::string emoji) {
            store_.react(room.str(), index, emoji);
            std::cout << "Реакция '" << emoji << "' добавлена.\n";
        });
    }
    
    ChatStore store_;
    CommandRegistry registry_;
};