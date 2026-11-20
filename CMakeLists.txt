cmake_minimum_required(VERSION 3.20)
project(CHAT LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra)

add_library(chat_core src/chat_core.cpp src/composition_root.cpp)
target_include_directories(chat_core PUBLIC includes)

add_executable(chat_cli src/main.cpp)
target_link_libraries(chat_cli PRIVATE chat_core)

add_executable(chat_tests src/tests.cpp)
target_link_libraries(chat_tests PRIVATE chat_core)

add_executable(benchmark src/benchmark.cpp)
target_link_libraries(benchmark PRIVATE chat_core)
target_compile_options(benchmark PRIVATE -O2)

enable_testing()
add_test(NAME engines COMMAND chat_tests)

add_executable(chat_tests_asan src/tests.cpp)
target_link_libraries(chat_tests_asan PRIVATE chat_core)
target_compile_options(chat_tests_asan PRIVATE -fsanitize=address,undefined -g)
target_link_options(chat_tests_asan PRIVATE -fsanitize=address,undefined)