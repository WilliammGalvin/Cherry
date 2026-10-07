#pragma once

#include "pit/ast/ast.hpp"
#include "pit/lexer/token.hpp"
#include "pit/sema/sema.hpp"

#include <cstdint>
#include <expected>
#include <span>
#include <string>

namespace pit::codegen {

struct codegen_error {
  std::string message;
  std::uint32_t line{1};
  std::uint32_t column{1};

  std::string to_str() const;
};

std::expected<std::string, codegen_error>
emit_ir(const ast::tree &tree, std::span<const lexer::token> tokens,
        const sema::analyzer &sema, std::string_view module_name);

std::expected<void, codegen_error>
emit_object(const ast::tree &tree, std::span<const lexer::token> tokens,
            const sema::analyzer &sema, std::string_view module_name,
            std::string_view path);

} // namespace pit::codegen
