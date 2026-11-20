cmake_minimum_required(VERSION 3.20)
project(CHAT LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra)

add_library(domain STATIC
    domain/src/room_state.cpp
    domain/src/event_serializer.cpp)
target_include_directories(domain PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/domain/include)


add_library(application STATIC application/src/chat_app.cpp)
target_include_directories(application PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/application/include)
target_link_libraries(application PUBLIC domain)


find_package(PkgConfig REQUIRED)
pkg_check_modules(RDKAFKA REQUIRED IMPORTED_TARGET rdkafka)

add_library(infrastructure STATIC
    infrastructure/src/composition_root.cpp
    infrastructure/src/kafka_producer.cpp)
target_include_directories(infrastructure PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/infrastructure/include)
target_link_libraries(infrastructure PUBLIC application domain PRIVATE PkgConfig::RDKAFKA)


add_executable(chat_cli apps/chat_cli.cpp)
target_link_libraries(chat_cli PRIVATE infrastructure)

add_executable(chat_tail apps/chat_tail.cpp)
target_link_libraries(chat_tail PRIVATE PkgConfig::RDKAFKA)

add_executable(benchmark apps/benchmark.cpp)
target_link_libraries(benchmark PRIVATE infrastructure)
target_compile_options(benchmark PRIVATE -O2)

add_executable(chat_tests tests/scenario_tests.cpp)
target_link_libraries(chat_tests PRIVATE infrastructure)

enable_testing()
add_test(NAME scenarios COMMAND chat_tests)

add_custom_target(check_layers
    COMMAND bash -c "! grep -rniE 'kafka|rdkafka' ${CMAKE_SOURCE_DIR}/domain ${CMAKE_SOURCE_DIR}/application"
    COMMAND ${CMAKE_COMMAND} -E echo "OK: ядро не знает о брокере"
    VERBATIM)