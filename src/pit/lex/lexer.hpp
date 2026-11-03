#pragma once

#include <common/io/source_manager.hpp>
#include <expected>
#include <vector>

#include <common/lexing/lex_error.hpp>
#include <common/lexing/source_cursor.hpp>
#include <common/lexing/token.hpp>
#include <pit/lex/token_type.hpp>

namespace pit::lex {

using namespace common::lexing;

class lexer {
public:
  using token_list = std::vector<token>;

  explicit lexer(common::io::source_id source_id,
                 common::io::source_info source) noexcept
      : _source_id(source_id), _cursor(source) {}

  lexer(const lexer &) = delete;
  lexer &operator=(const lexer &) = delete;

  lexer(lexer &&) = delete;
  lexer &operator=(lexer &&) = delete;

  std::expected<void, lex_error> lex();

  const token_list &tokens() const noexcept { return _tokens; }

private:
  using lex_result = std::expected<token, lex_error>;

  common::io::source_id _source_id;
  common::io::source_info _source;
  source_cursor _cursor;
  token_list _tokens{};

  lex_result _lex_identifier_or_keyword();

  lex_result _lex_number();

  lex_result _lex_directive();

  lex_result _lex_string_literal();

  lex_result _lex_symbol();

  void _push_token(token_type type, std::uint32_t start) {
    const auto end = _cursor.offset();
    _tokens.push_back(token{_source_id,
                            static_cast<common::lexing::base_token_type>(type),
                            start, end - start});
  }

  void _push_token(token_type type) { _push_token(type, _cursor.offset()); }

  void _reset() noexcept {
    _cursor.reset();
    _tokens.clear();
  }

  lex_result _skip_comments_and_whitespace();

  lex_error _error_here(lex_error_kind kind) const noexcept {
    return lex_error{kind, _cursor.offset(), _cursor.line(), _cursor.column()};
  }
};

} // namespace pit::lex
