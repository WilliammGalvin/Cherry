#pragma once

#include "pit/ast/ast.hpp"
#include "pit/lexer/token.hpp"

#include <cstdint>
#include <expected>
#include <span>
#include <string>
#include <vector>

namespace pit::parser {

struct parse_error {
  std::string message;
  std::uint32_t offset{0};
  std::uint32_t line{1};
  std::uint32_t column{1};

  std::string to_str() const;
};

using parse_result = std::expected<ast::tree, parse_error>;

class parser {
public:
  explicit parser(std::span<const lexer::token> tokens) noexcept
      : tokens_(tokens) {}

  parse_result parse();

private:
  struct error_signal {
    parse_error error;
  };

  std::span<const lexer::token> tokens_;
  ast::tree tree_;
  std::uint32_t index_{0};

  std::vector<ast::node_id> scratch_;

  // --- token access -------------------------------------------------------
  const lexer::token &peek(std::uint32_t ahead = 0) const noexcept;
  lexer::token_type peek_type(std::uint32_t ahead = 0) const noexcept;
  ast::token_id here() const noexcept;
  bool at_end() const noexcept;

  bool check(lexer::token_type type) const noexcept;
  ast::token_id advance() noexcept;
  bool match(lexer::token_type type) noexcept;
  ast::token_id expect(lexer::token_type type, std::string_view what);

  [[noreturn]] void fail(std::string message) const;

  // --- declarations -------------------------------------------------------
  ast::node_id parse_decl();
  ast::node_id parse_fn_decl();
  ast::node_id parse_param();
  ast::node_id parse_type();

  // --- statements ---------------------------------------------------------
  ast::node_id parse_block();
  ast::node_id parse_stmt();
  ast::node_id parse_local_decl();
  ast::node_id parse_assign_or_expr_stmt();
  ast::node_id parse_if();
  ast::node_id parse_while();
  ast::node_id parse_return();

  // --- expressions --------------------------------------------------------
  ast::node_id parse_expr();
  ast::node_id parse_binary(int min_precedence);
  ast::node_id parse_unary();
  ast::node_id parse_postfix();
  ast::node_id parse_primary();
  ast::node_id parse_call(ast::node_id callee);

  bool starts_declaration() const noexcept;
};

} // namespace pit::parser
