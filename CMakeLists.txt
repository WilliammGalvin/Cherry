cmake_minimum_required(VERSION 3.30)
project(Cherry)

set(CMAKE_CXX_STANDARD 20)

add_executable(Cherry main.cpp
        lexer/include/lexer.hpp
        lexer/include/token.hpp
        lexer/src/lexer.cpp
        lexer/include/lex_error.hpp
        lexer/src/lex_error.cpp
        lexer/src/token.cpp)
