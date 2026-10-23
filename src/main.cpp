#include "chat_app.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

void demonstrateConcepts() {
    std::cout << "=== Демонстрация концепций (NickLike) ===\n";
    ChatStore store;
    store.create_room(std::string("room1"));       
    store.create_room(std::string_view("room2"));  
    store.create_room("room3");                    
    std::cout << "Создано комнат: " << store.room_count() << "\n";
    std::cout << "(int в create_room не примет — концепция NickLike отсекает)\n\n";
}

void demonstrateRanges() {
    std::cout << "=== Демонстрация ranges: ленивость конвейера ===\n";
    ChatStore store;
    store.create_room("test");
    for (int i = 0; i < 15; ++i) {
        std::string author = (i % 2 == 0) ? "Alice" : "Bob";
        store.post("test", make_text(author, "Message " + std::to_string(i)));
    }
    std::cout << "--- Берём первые 3 от Alice (take=3 из 15) ---\n";
    auto history = store.history_with_filters("test", "Alice", 3);
    std::cout << "Результат (" << history.size() << "):\n";
    for (const auto& m : history) std::cout << "  - " << m << "\n";
    std::cout << "Считай строки [DEBUG]: их ~6, а не 15 => конвейер ленив.\n\n";
}

void demonstrateProjection() {
    std::cout << "=== Демонстрация проекции (сортировка по длине ника) ===\n";
    ChatStore store;
    store.create_room("team");
    auto alice = std::make_shared<User>("Alice");
    auto bob   = std::make_shared<User>("Bob");
    auto charlie = std::make_shared<User>("Charlie");
    auto dave  = std::make_shared<User>("Dave");
    store.add_participant("team", alice);
    store.add_participant("team", bob);
    store.add_participant("team", charlie);
    store.add_participant("team", dave);
    auto sorted = store.sorted_participants_by_nickname_length("team");
    std::cout << "По возрастанию длины ника:\n";
    for (const auto& n : sorted)
        std::cout << "  - " << n << " (len=" << n.size() << ")\n";
    std::cout << "\n";
}

void demonstrateMessageId() {
    std::cout << "=== Демонстрация MessageId и оператора <=> ===\n";
    auto now = std::chrono::system_clock::now();
    MessageId id1(1, now), id2(2, now), id3(1, now);
    std::cout << std::boolalpha;
    std::cout << "id1<id2 : " << (id1 <  id2) << " (ожид. true)\n";
    std::cout << "id1>id2 : " << (id1 >  id2) << " (ожид. false)\n";
    std::cout << "id1==id3: " << (id1 == id3) << " (ожид. true)\n";
    std::cout << "id1!=id2: " << (id1 != id2) << " (ожид. true)\n";
    std::cout << "id1<=id3: " << (id1 <= id3) << " (ожид. true)\n";
    std::cout << "id1>=id3: " << (id1 >= id3) << " (ожид. true)\n";

    MessageIdEx ex1(1, now, "Alice");
    MessageIdEx ex2(1, now, "Bob");   
    std::cout << "ex1==ex2 (author вне сравнения): " << (ex1 == ex2)
              << " (ожид. true)\n\n";
}

void demonstrateDanglingRange() {
    std::cout << "=== Демонстрация висячего диапазона ===\n";
    auto range = dangling_range_demo();   
    std::cout << "Функция скомпилировалась => компилятор висячий view НЕ поймал.\n";
    std::cout << "Итерация по range сейчас = UB (temp уничтожен), поэтому НЕ\n";
    std::cout << "итерируемся, чтобы не ронять демо. Под ASan это упало бы как\n";
    std::cout << "heap-use-after-free. Вывод: владелец данных обязан жить\n";
    std::cout << "дольше view.\n\n";
    (void)range;
}

int main() {
    demonstrateConcepts();
    demonstrateRanges();
    demonstrateProjection();
    demonstrateMessageId();
    demonstrateDanglingRange();

    ChatApp app;
    std::cout << "=== Интерактивный чат ===\n";
    std::cout << "Команды: /join /leave /msg /who /history /topic /react\n";
    std::cout << "Рассылка: /msg #r1 #r2 Текст\n";
    std::cout << "Введите 'exit' для выхода.\n\n";

    std::string line;
    while (std::getline(std::cin, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line == "exit") break;
        if (line.empty()) continue;
        try {
            auto cmd = CommandParser::parse(line);
            if (cmd) app.execute(*cmd);
            else std::cout << "Пустой ввод.\n";
        } catch (const ChatError& e) {
            std::cout << "[ОШИБКА] " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cout << "[ОШИБКА] " << e.what() << "\n";
        }
        std::cout << "\n";
    }
    return 0;
}