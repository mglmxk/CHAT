#include "composition_root.hpp"
#include "colors.hpp"
#include <iostream>
#include <string>

static void print_banner(const std::string& engine) {
    std::cout << Colors::CYAN << Colors::BOLD;
    std::cout << R"(
    ╔═══════════════════════════════════════════╗
    ║          🚀 CHAT APPLICATION 🚀           ║
    ╚═══════════════════════════════════════════╝
    )" << Colors::RESET;
    std::cout << Colors::MAGENTA << "    Движок: " << engine
              << (engine == "hash" ? " (собственная хеш-таблица)" : " (std::map)")
              << "\n" << Colors::RESET;
}

static void print_help() {
    std::cout << "\n" << Colors::YELLOW << Colors::BOLD << "📖 Доступные команды:\n" << Colors::RESET;
    std::cout << Colors::CYAN << "  /join #room" << Colors::RESET << "               - Создать комнату\n";
    std::cout << Colors::CYAN << "  /leave #room" << Colors::RESET << "              - Покинуть комнату\n";
    std::cout << Colors::CYAN << "  /msg #room текст" << Colors::RESET << "          - Отправить сообщение\n";
    std::cout << Colors::CYAN << "  /history #room" << Colors::RESET << "            - История комнаты\n";
    std::cout << Colors::CYAN << "  /history-by #room автор" << Colors::RESET << "   - История по автору\n";
    std::cout << Colors::CYAN << "  /topic #room тема" << Colors::RESET << "         - Изменить тему\n";
    std::cout << Colors::CYAN << "  /react #room N эмодзи" << Colors::RESET << "     - Добавить реакцию\n";
    std::cout << Colors::CYAN << "  /rooms" << Colors::RESET << "                    - Список комнат\n";
    std::cout << Colors::CYAN << "  /who" << Colors::RESET << "                      - Кто в комнатах\n";
    std::cout << Colors::CYAN << "  /nick имя" << Colors::RESET << "                 - Сменить ник\n";
    std::cout << Colors::CYAN << "  /kick #room имя" << Colors::RESET << "           - Выгнать пользователя\n";
    std::cout << Colors::CYAN << "  /help" << Colors::RESET << "                     - Эта справка\n";
    std::cout << Colors::CYAN << "  /exit" << Colors::RESET << "                     - Выйти\n\n";
}

int main(int argc, char* argv[]) {
    std::string engine = "std", log_path;
    for (int i = 1; i < argc; ++i) {
        std::string a(argv[i]);
        if (a.rfind("--engine=", 0) == 0) engine = a.substr(9);
        else if (a.rfind("--log=", 0) == 0) log_path = a.substr(6);
    }

    auto c = build_app(engine, log_path);

    print_banner(engine);
    print_help();

    std::cout << Colors::GREEN << "✅ Чат готов к работе!\n" << Colors::RESET;
    std::cout << "Введите " << Colors::BOLD << "/help" << Colors::RESET << " для списка команд.\n\n";

    std::string line;
    while (true) {
        std::cout << Colors::MAGENTA << "👉 " << Colors::RESET;
        if (!std::getline(std::cin, line)) break;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;

        if (line == "/exit" || line == "exit") {
            std::cout << Colors::YELLOW << "👋 До свидания!\n" << Colors::RESET;
            break;
        }
        if (line == "/help" || line == "help") { print_help(); continue; }

        c.app->execute(line);
        std::cout << "\n";
    }
    return 0;
}