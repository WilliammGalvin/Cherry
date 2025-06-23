#include "cherry/semantic/semantic_error.hpp"

using namespace cherry::semantic;

SemanticError::SemanticError(std::string msg)
    : message("=== 🍒 Cherry Semantic Error ===\n" + std::move(msg)) {}

const char* SemanticError::what() const noexcept {
    return message.c_str();
}
