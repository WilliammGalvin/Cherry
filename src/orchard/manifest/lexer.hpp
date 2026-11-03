#pragma once

#include <common/cursor/cursor.hpp>
#include <common/io/source_manager.hpp>
#include <common/lexing/lex_error.hpp>
#include <common/lexing/token.hpp>
#include <expected>
#include <vector>

namespace orchard::manifest {

using namespace common::lexing;
using namespace common::cursor;

class lexer {
public:
  using token_vector = std::vector<token>;

  explicit lexer(common::io::source_info source) : _cursor{source.text} {}

  lexer(const lexer &) = delete;
  lexer &operator=(const lexer &) = delete;

  lexer(lexer &&) = delete;
  lexer &operator=(lexer &&) = delete;

  std::expected<std::vector<token>, lex_error> lex();

private:
  using lex_result = std::expected<token, lex_error>;

  cursor<char> _cursor;
  token_vector _tokens{};

  lex_result _lex_symbol();

  lex_result _lex_identifier();

  lex_result _lex_string();

  lex_result _lex_number();

  void _skip_whitespace_and_comments();
};

} // namespace orchard::manifest
