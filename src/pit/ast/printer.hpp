#pragma once

#include "pit/ast/ast.hpp"
#include "pit/lexer/token.hpp"

#include <span>
#include <string>

namespace pit::ast {

std::string dump(const tree &t, std::span<const lexer::token> tokens);

} // namespace pit::ast
