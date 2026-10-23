cmake_minimum_required(VERSION 3.20)
project(CHAT VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   

add_compile_options(-Wall -Wextra -Wshadow)

add_library(chat_core
    src/chat_core.cpp
)
target_include_directories(chat_core PUBLIC includes)
target_compile_features(chat_core PUBLIC cxx_std_23)  

add_executable(chat_cli src/main.cpp)
target_link_libraries(chat_cli PRIVATE chat_core)
target_compile_features(chat_cli PRIVATE cxx_std_23)   