#pragma once

#include "token.hpp"
#include <cstddef>
#include <expected>
#include <vector>

namespace cherry::lexer {

enum class lex_error {
  unexpected_end_of_input,
  unidentifiable_token,
};

class lexer {
public:
  std::vector<token> lex_file(std::string_view &file_name);

private:
  std::vector<token> tokens_;
  std::string line_source_;
  std::size_t index_{};

  bool in_block_comment_ = false;

  [[nodiscard]] char peek(std::size_t pos = 0) const noexcept;
  std::expected<char, lex_error> consume() noexcept;
  std::expected<void, lex_error> advance(std::size_t pos = 1);
  [[nodiscard]] bool is_empty() const noexcept;
  [[nodiscard]] bool match_front(const std::string &str) const noexcept;

  bool match_symbol();
  bool match_number_literal();
  bool match_string_literal();
  bool match_keyword();
  bool match_identifier();
  bool match_directive();

  std::expected<void, lex_error> lex_line();
};

} // namespace cherry::lexer
