cmake_minimum_required(VERSION 3.28)
project(cherry LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(cherry_lexer STATIC
    src/lexer/lexer.cpp
    src/lexer/token.cpp
    src/lexer/token_type.cpp
)
target_include_directories(cherry_lexer PUBLIC src)

add_executable(cherry src/main.cpp)
target_link_libraries(cherry PRIVATE cherry_lexer)
