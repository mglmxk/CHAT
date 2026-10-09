#include "chat_app.hpp"
#include <iostream>
#include <string>
#include <cassert>

void demonstrateSmartPointers() {
    std::cout << "=== Демонстрация умных указателей ===\n";
    
    std::vector<std::unique_ptr<IMessage>> messages;
    messages.push_back(make_text("Никита", "Привет!"));
    messages.push_back(make_system("Системное уведомление"));
    messages.push_back(make_reaction("Миша", "", 5));
    
    for (const auto& msg : messages) {
        std::cout << "  [" << msg->typeName() << "] " 
                  << msg->author() << ": " << msg->render() << "\n";
    }
    std::cout << "\n";

    std::cout << "--- Пользователи ---\n";
    UserPtr nikita = std::make_shared<User>("Никита");
    UserPtr misha = std::make_shared<User>("Миша");
    
    ChatStore store;
    store.create_room("general");
    store.add_participant("general", nikita);
    store.add_participant("general", misha);
    
    std::cout << "Активных участников: " 
              << store.active_participants("general") << "\n";
    
    nikita.reset();
    std::cout << "После удаления Nikita: " 
              << store.active_participants("general") << "\n";
    
    store.cleanup_participants("general");
    std::cout << "После cleanup: " 
              << store.active_participants("general") << "\n\n";
    std::cout << "--- sizeof unique_ptr с делетерами ---\n";
    std::cout << "sizeof(unique_ptr<FILE, FileCloser>): " 
              << sizeof(UniqueFile) << " байт\n";
    std::cout << "sizeof(unique_ptr<FILE, void(*)(FILE*)>): " 
              << sizeof(std::unique_ptr<FILE, void(*)(FILE*)>) << " байт\n";
    std::cout << "(функтор = 8 байт, указатель на функцию = 16 байт)\n\n";

    std::cout << "--- RAII журнал ---\n";
    {
        AppendOnlyLog log("/tmp/chat_test.log");
        log.append("Тестовая запись 1");
        log.append("Тестовая запись 2");
        std::cout << "Журнал закрыт при выходе из блока\n";
    }
    std::cout << "\n";
}

int main() {
    demonstrateSmartPointers();

    ChatApp app;
    
    std::cout << "=== Интерактивный чат ===\n";
    std::cout << "Команды: /join, /leave, /msg, /who, /history, /topic, /react\n";
    std::cout << "Пример рассылки: /msg #room1 #room2 #room3 Текст сообщения\n";
    std::cout << "Введите 'exit' для выхода.\n\n";

    std::string line;
    while (std::getline(std::cin, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line == "exit") {
            break;
        }

        if (line.empty()) {
            continue;
        }

        try {
            auto cmd = CommandParser::parse(line);
            if (cmd) {
                app.execute(*cmd);
            } else {
                std::cout << "Пустой ввод.\n";
            }
        } catch (const ChatError& e) {
            std::cout << "[ОШИБКА] " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cout << "[ОШИБКА] " << e.what() << "\n";
        }

        std::cout << "\n";
    }

    return 0;
}