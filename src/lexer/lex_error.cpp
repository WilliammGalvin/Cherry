#include "cherry/lexer/lex_error.hpp"

#include <utility>

using namespace cherry::lexer;

LexError::LexError(std::string msg)
    : message("=== 🍒 Cherry Lex Error ===\n" + std::move(msg)) {}

const char* LexError::what() const noexcept {
    return message.c_str();
}