#include "ichat_store.hpp"
#include "std_engine.hpp"
#include "hash_engine.hpp"
#include "message_factory.hpp"
#include "adapters/fake_clock.hpp"
#include "adapters/memory_log_sink.hpp"
#include "user.hpp"
#include <iostream>
#include <string>
#include <algorithm>

static int failures = 0;
#define CHECK(cond) do { if(!(cond)) { \
    std::cout << "  FAIL " << __LINE__ << ": " #cond "\n"; ++failures; } } while(0)

static void run_suite(const char* label, IChatStore& store,
                      MessageFactory& factory, FakeClock& clock, MemoryLogSink& sink) {
    std::cout << "== " << label << " ==\n";

    store.create_room("#a");
    store.create_room("#b");
    CHECK(store.room_count() == 2);
    CHECK(store.room_exists("#a"));
    CHECK(!store.room_exists("#zzz"));

    bool threw = false;
    try { store.create_room("#a"); } catch (const ChatError&) { threw = true; }
    CHECK(threw);

    threw = false;
    try { store.post("#zzz", factory.text("Никита", "x")); }
    catch (const UnknownRoomError&) { threw = true; }
    CHECK(threw);
    CHECK(!store.room_exists("#zzz"));   

    clock.advance(std::chrono::seconds(1));
    store.post("#a", factory.text("Никита", "m0"));
    clock.advance(std::chrono::seconds(1));
    store.post("#a", factory.text("Сема", "m1"));
    clock.advance(std::chrono::seconds(1));
    store.post("#a", factory.text("Никита", "m2"));

    auto all = store.history("#a", HistoryQuery{});
    CHECK(all.size() == 3);
    CHECK(all[0].render() == "m0");
    CHECK(all[0].id().timestamp() < all[2].id().timestamp());  

    auto by_author = store.history("#a", HistoryQuery{"Никита", 0});
    CHECK(by_author.size() == 2);
    auto limited = store.history("#a", HistoryQuery{"Никита", 1});
    CHECK(limited.size() == 1);

  
    store.post("#a", factory.reaction("Миша", "👍"));
    store.react("#a", 3, "👍");
    CHECK(store.history("#a", HistoryQuery{}).back().render() == "👍 x2");

    threw = false;
    try { store.react("#a", 0, "👍"); } catch (const TypeMismatchError&) { threw = true; }
    CHECK(threw);

    auto before = store.history("#b", HistoryQuery{}).size();
    threw = false;
    try { store.multi_post({"#b", "#missing"}, factory.text("Никита", "broadcast")); }
    catch (const UnknownRoomError&) { threw = true; }
    CHECK(threw);
    CHECK(store.history("#b", HistoryQuery{}).size() == before);

    auto msg = factory.text("Никита", "ok");
    store.multi_post({"#a", "#b"}, msg);
    CHECK(store.history("#b", HistoryQuery{}).size() == before + 1);

  
    store.create_room("#small");
    for (int i = 0; i < 150; ++i) store.post("#small", factory.text("Никита", std::to_string(i)));
    auto h = store.history("#small", HistoryQuery{});
    CHECK(h.size() == 100);
    CHECK(h.front().render() == "50");

    auto nikita = std::make_shared<User>("Никита");
    auto sema   = std::make_shared<User>("Сема");
    store.add_participant("#a", nikita);
    { auto tmp = std::make_shared<User>("Миша"); store.add_participant("#a", tmp); }
    store.add_participant("#a", sema);
    CHECK(store.active_participants("#a") == 2);
    store.cleanup_participants("#a");
    CHECK(store.active_participants("#a") == 2);
    auto sorted = store.nicks_by_length("#a");
    CHECK(sorted.front().size() <= sorted.back().size());

    auto names = store.room_names();
    CHECK(std::is_sorted(names.begin(), names.end()));

    CHECK(!sink.lines().empty()); 
}

int main() {
    {
        FakeClock clock; MemoryLogSink sink;
        MessageFactory factory(clock);
        StdEngine engine(sink);
        run_suite("StdEngine", engine, factory, clock, sink);
    }
    {
        FakeClock clock; MemoryLogSink sink;
        MessageFactory factory(clock);
        HashEngine engine(sink);
        run_suite("HashEngine", engine, factory, clock, sink);
    }
    std::cout << (failures ? "\nЕСТЬ ПАДЕНИЯ\n" : "\nВСЕ ТЕСТЫ ПРОШЛИ НА ОБОИХ ДВИЖКАХ\n");
    return failures ? 1 : 0;
}