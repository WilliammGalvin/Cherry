#include "cherry/parser/parse_error.hpp"

using namespace cherry::parser;

ParseError::ParseError(std::string msg)
    : message(std::move(msg)) {}

const char* ParseError::what() const noexcept {
    return ("=== 🍒 Cherry Parse Error ===\n" + message).c_str();
}
