#include "cherry/ir/ir_error.hpp"

#include <utility>

using namespace cherry::ir;

IRError::IRError(std::string msg)
    : message("=== 🍒 Cherry IR Error ===\n" + std::move(msg)) {}

const char* IRError::what() const noexcept {
    return message.c_str();
}