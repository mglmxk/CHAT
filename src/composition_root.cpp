#include "composition_root.hpp"
#include "adapters/system_clock.hpp"
#include "adapters/file_log_sink.hpp"
#include "adapters/null_log_sink.hpp"
#include "adapters/console_presenter.hpp"
#include "std_engine.hpp"
#include "hash_engine.hpp"

AppComponents build_app(std::string_view engine_kind, std::string_view log_path) {
    AppComponents c;
    c.clock = std::make_unique<SystemClock>();
    c.sink = log_path.empty()
           ? std::unique_ptr<ILogSink>(std::make_unique<NullLogSink>())
           : std::unique_ptr<ILogSink>(std::make_unique<FileLogSink>(std::string(log_path)));
    c.presenter = std::make_unique<ConsolePresenter>();
    c.factory = std::make_unique<MessageFactory>(*c.clock);
    c.engine = (engine_kind == "hash")
             ? std::unique_ptr<IChatStore>(std::make_unique<HashEngine>(*c.sink))
             : std::unique_ptr<IChatStore>(std::make_unique<StdEngine>(*c.sink));
    c.app = std::make_unique<ChatApp>(*c.engine, *c.factory, *c.presenter);
    return c;
}