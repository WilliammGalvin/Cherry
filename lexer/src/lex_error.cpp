#include <utility>

#include "../include/lex_error.hpp"

using namespace lexer;


LexError::LexError(std::string msg)
    : message(std::move(msg)) {}

const char* LexError::what() const noexcept {
    return message.c_str();
}