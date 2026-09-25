#include "chat_store.hpp"
#include "owning_buffer.hpp"
#include <iostream>
#include <string>

int main() {
    ChatStore store;
    store.create_room("general");
    store.create_room("random");

    store.post("general", Message("Миша", "Привет всем!"));
    store.post("general", Message("Никита", "Привет, Миша!"));
    store.post("random", Message("семен", "Случайное сообщение"));

    int total_messages_ref = 0;
    store.for_each_room([&total_messages_ref](const std::string& name, const std::vector<Message>& msgs) {
        total_messages_ref += msgs.size();
        std::cout << "Комната '" << name << "': " << msgs.size() << " сообщений\n";
    });
    std::cout << "Всего сообщений (по ссылке): " << total_messages_ref << "\n\n";

    int total_messages_val = 0;
    store.for_each_room([total_messages_val](const std::string& name, const std::vector<Message>& msgs) mutable {
        total_messages_val += msgs.size();
        std::cout << "Комната '" << name << "': " << msgs.size() << " сообщений\n";
    });
    std::cout << "Всего сообщений: " << total_messages_val << "\n\n";

    std::cout << "=== OwningBuffer ===\n";
    OwningBuffer buf1(5);
    buf1.data()[0] = 'A';
    buf1.data()[1] = 'B';
    
    OwningBuffer buf2(buf1);  
    buf2.data()[0] = 'X';    
    
    std::cout << "buf1[0]: " << buf1.data()[0] << "\n";
    std::cout << "buf2[0]: " << buf2.data()[0] << "\n";
    
    buf1 = buf1;
    std::cout << "После самоприсваивания buf1[0]: " << buf1.data()[0] << "\n";
    
    OwningBuffer buf3(std::move(buf2));
    std::cout << "buf3 size: " << buf3.size() << "\n";
    std::cout << "buf2 size: " << buf2.size() << "\n";

    return 0;
}