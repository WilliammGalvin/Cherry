#pragma once

#include "pit/lexer/token.hpp"

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>
#include <vector>

namespace pit::lexer {

enum class lex_error_kind {
  unterminated_string,
  unterminated_block_comment,
  invalid_escape_sequence,
  unexpected_character,
};

std::string_view lex_error_kind_to_str(lex_error_kind kind) noexcept;

struct lex_error {
  lex_error_kind kind;
  std::uint32_t offset;
  std::uint32_t line;
  std::uint32_t column;

  std::string to_str() const;
};

using lex_result = std::expected<std::vector<token>, lex_error>;

class lexer {
public:
  explicit lexer(std::string_view source) noexcept : source_(source) {}

  lex_result lex();

private:
  std::string_view source_;
  std::vector<token> tokens_{};
  std::uint32_t index_{0};
  std::uint32_t line_{1};
  std::uint32_t line_start_{0};

  bool at_end() const noexcept { return index_ >= source_.size(); }

  char peek(std::uint32_t ahead = 0) const noexcept;

  bool starts_with(std::string_view text) const noexcept;

  std::uint32_t column() const noexcept { return index_ - line_start_ + 1; }

  char bump() noexcept;
  void advance(std::uint32_t count) noexcept;

  void push(token_type type, std::uint32_t start, std::uint32_t start_line,
            std::uint32_t start_column);
  [[nodiscard]] lex_error error_here(lex_error_kind kind) const noexcept;

  std::expected<void, lex_error> skip_comments_and_whitespace();

  void lex_identifier_or_keyword();
  void lex_number();
  void lex_directive();
  std::expected<void, lex_error> lex_string_literal();
  std::expected<void, lex_error> lex_symbol();
};

} // namespace pit::lexer
