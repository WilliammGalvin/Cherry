#include "cherry/lexer/lex_error.hpp"

#include <utility>

using namespace cherry::lexer;

LexError::LexError(std::string msg)
    : message(std::move(msg)) {}

const char* LexError::what() const noexcept {
    return ("=== 🍒 Cherry Lex Error ===\n" + message).c_str();
}