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
#include "chat_app.hpp"
#include "adapters/vector_event_publisher.hpp"

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

namespace {
struct SilentPresenter final : IPresenter {
    void success(std::string_view) override {}
    void info(std::string_view) override {}
    void warn(std::string_view) override {}
    void error(std::string_view) override {}
    void room_list(const std::vector<std::string>&) override {}
    void history(std::string_view, const std::vector<Message>&) override {}
};
}

static void run_event_suite(const char* label, IChatStore& store,
                            MessageFactory& factory, FakeClock& clock,
                            MemoryLogSink& sink) {
    std::cout << "== events: " << label << " ==\n";
    SilentPresenter ui;
    VectorEventPublisher pub;
    ChatApp app(store, factory, ui, pub, clock);

    app.execute("/nick Никита");
    app.execute("/join #ev");
    app.execute("/msg #ev Привет");
    app.execute("/leave #ev");

    const auto& ev = pub.events();
    CHECK(ev.size() == 3);
    if (ev.size() == 3) {
        CHECK(ev[0].type == "UserJoined");
        CHECK(ev[1].type == "MessagePosted");
        CHECK(ev[2].type == "UserLeft");
        CHECK(ev[0].key == "#ev");
        CHECK(ev[1].key == "#ev");  
        CHECK(ev[2].key == "#ev");
        CHECK(ev[1].payload.find("\"author\":\"Никита\"") != std::string::npos);
        CHECK(ev[1].payload.find("\"text\":\"Привет\"") != std::string::npos);
    }

    
    pub.clear();
    app.execute("/msg #missing x");
    CHECK(pub.events().empty());
}

int main() {
    {
        FakeClock clock; MemoryLogSink sink;
        MessageFactory factory(clock);
        StdEngine engine(sink);
        run_suite("StdEngine", engine, factory, clock, sink);
        run_event_suite("StdEngine", engine, factory, clock, sink);
        run_event_suite("HashEngine", engine, factory, clock, sink);
    }
    {
        FakeClock clock; MemoryLogSink sink;
        MessageFactory factory(clock);
        HashEngine engine(sink);
        run_suite("HashEngine", engine, factory, clock, sink);
        run_event_suite("StdEngine", engine, factory, clock, sink);
        run_event_suite("HashEngine", engine, factory, clock, sink);
    }
    std::cout << (failures ? "\nЕСТЬ ПАДЕНИЯ\n" : "\nВСЕ ТЕСТЫ ПРОШЛИ НА ОБОИХ ДВИЖКАХ\n");
    return failures ? 1 : 0;
}