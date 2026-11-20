#include "composition_root.hpp"
#include "colors.hpp"
#include <iostream>
#include <string>

static void print_banner(const AppOptions& o, const std::string& mode) {
    std::cout << Colors::CYAN << Colors::BOLD;
    std::cout << R"(
    ╔═══════════════════════════════════════════╗
    ║          🚀 CHAT APPLICATION  🚀          ║
    ╚═══════════════════════════════════════════╝
    )" << Colors::RESET;
    std::cout << Colors::MAGENTA << "    Движок: " << o.engine
              << "   События: " << mode << "\n" << Colors::RESET;
}

static void print_help() {
    std::cout << "\n" << Colors::YELLOW << Colors::BOLD << "📖 Команды:\n" << Colors::RESET;
    std::cout << Colors::CYAN << "  /join #room" << Colors::RESET << "               - Войти в комнату (UserJoined)\n";
    std::cout << Colors::CYAN << "  /leave #room" << Colors::RESET << "              - Выйти (UserLeft)\n";
    std::cout << Colors::CYAN << "  /msg #room текст" << Colors::RESET << "          - Сообщение (MessagePosted)\n";
    std::cout << Colors::CYAN << "  /history #room" << Colors::RESET << "            - История\n";
    std::cout << Colors::CYAN << "  /history-by #room автор" << Colors::RESET << "   - История по автору\n";
    std::cout << Colors::CYAN << "  /topic #room тема" << Colors::RESET << "         - Тема\n";
    std::cout << Colors::CYAN << "  /react #room N эмодзи" << Colors::RESET << "     - Реакция\n";
    std::cout << Colors::CYAN << "  /rooms /who" << Colors::RESET << "               - Список комнат\n";
    std::cout << Colors::CYAN << "  /nick имя" << Colors::RESET << "                 - Сменить ник\n";
    std::cout << Colors::CYAN << "  /kick #room имя" << Colors::RESET << "           - Выгнать (UserLeft)\n";
    std::cout << Colors::CYAN << "  /help /exit" << Colors::RESET << "\n\n";
}

int main(int argc, char* argv[]) {
    AppOptions o;
    for (int i = 1; i < argc; ++i) {
        std::string a(argv[i]);
        if      (a.rfind("--engine=", 0)  == 0) o.engine   = a.substr(9);
        else if (a.rfind("--log=", 0)     == 0) o.log_path = a.substr(6);
        else if (a.rfind("--events=", 0)  == 0) o.events   = a.substr(9);
        else if (a.rfind("--brokers=", 0) == 0) o.brokers  = a.substr(10);
        else if (a.rfind("--topic=", 0)   == 0) o.topic    = a.substr(8);
    }

    auto c = build_app(o);
    print_banner(o, c.events_mode);
    print_help();
    std::cout << Colors::GREEN << "✅ Готов. /help — справка.\n\n" << Colors::RESET;

    std::string line;
    while (true) {
        std::cout << Colors::MAGENTA << "👉 " << Colors::RESET;
        if (!std::getline(std::cin, line)) break;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        if (line == "/exit" || line == "exit") break;
        if (line == "/help" || line == "help") { print_help(); continue; }
        c.app->execute(line);
        std::cout << "\n";
    }
    std::cout << Colors::YELLOW << "👋 До свидания!\n" << Colors::RESET;
}