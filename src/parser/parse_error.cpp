#include "cherry/parser/parse_error.hpp"

using namespace cherry::parser;

ParseError::ParseError(std::string msg)
    : message("=== 🍒 Cherry Parse Error ===\n" + std::move(msg)) {}

const char* ParseError::what() const noexcept {
    return message.c_str();
}
