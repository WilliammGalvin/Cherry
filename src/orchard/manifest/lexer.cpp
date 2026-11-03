#include "common/lexing/lex_error.hpp"
#include "orchard/manifest/token_type.hpp"
#include <common/cursor/predicates.hpp>
#include <expected>
#include <orchard/manifest/lexer.hpp>
#include <string_view>

namespace orchard::manifest {

std::expected<lexer::token_vector, lex_error> lexer::lex() {
  const auto try_lex = [&](bool cond,
                           auto lex_fn) -> std::expected<bool, lex_error> {
    if (!cond)
      return false;

    const auto res = (this->*lex_fn)();
    if (!res)
      return std::unexpected(res.error());

    _tokens.push_back(res.value());
    return true;
  };

  const auto next = [&](bool cond, auto lex_fn) {
    return [=](bool done) -> std::expected<bool, lex_error> {
      return done ? std::expected<bool, lex_error>{true}
                  : try_lex(cond, lex_fn);
    };
  };

  while (!_cursor.at_end()) {
    _skip_whitespace_and_comments();
    if (_cursor.at_end())
      break;

    const char c = _cursor.peek();
    const auto handled =
        try_lex(c == '"' || c == '\'', &lexer::_lex_string)
            .and_then(next(is_punct(c), &lexer::_lex_symbol))
            .and_then(next(is_digit(c), &lexer::_lex_number))
            .and_then(next(is_ident_start(c), &lexer::_lex_identifier));

    if (!handled)
      return std::unexpected(handled.error());

    if (*handled)
      continue;

    return std::unexpected(lex_error{/* unexpected character c */});
  }

  return _tokens;
}

lexer::lex_result lexer::_lex_symbol() {
  const token_type type = symbol_to_token_type(_cursor.peek());
  if (type == token_type::invalid)
    return std::unexpected(lex_error{/* invalid symbol */});

  _cursor.advance();
  return {}; // TODO: token return
}

lexer::lex_result lexer::_lex_identifier() {
  if (!is_ident_start(_cursor.peek()))
    return std::unexpected(lex_error{/* expected identifier start */});

  const auto start_pos = _cursor.offset();
  _cursor.advance_while(is_ident_continue);
  const auto end_pos = _cursor.offset();
  const auto ident_value = _cursor.slice(start_pos, end_pos);

  const token_type kw = keyword_to_token_type(ident_value);
  const bool is_kw = kw != token_type::invalid;

  return {}; // TODO: token return
}

lexer::lex_result lexer::_lex_string() {
  if (_cursor.peek() != '"' && _cursor.peek() != '\'')
    return std::unexpected(lex_error{/* expected string start */});

  const char quote_char = _cursor.peek();
  _cursor.advance(); // consume opening quote
                     //
  const auto start_pos = _cursor.offset();
  _cursor.advance_while(
      [quote_char](char c) { return c != quote_char && !is_newline(c); });
  if (_cursor.at_end() || _cursor.peek() != quote_char)
    return std::unexpected(lex_error{/* unterminated string */});

  const auto end_pos = _cursor.offset();
  _cursor.advance(); // consume closing quote
  const auto str_value = _cursor.slice(start_pos, end_pos);
  return {}; // TODO: token return
}

lexer::lex_result lexer::_lex_number() {
  if (!is_digit(_cursor.peek()))
    return std::unexpected(lex_error{/* expected digit */});

  bool is_float = false;
  const auto start_pos = _cursor.offset();
  while (!_cursor.at_end() &&
         (is_digit(_cursor.peek()) || _cursor.peek() == '.')) {
    if (_cursor.peek() == '.') {
      if (is_float)
        return std::unexpected(lex_error{/* multiple decimal points */});

      is_float = true;
    }

    _cursor.advance();
  }

  const auto end_pos = _cursor.offset();
  const auto num_value = _cursor.slice(start_pos, end_pos);
  return {}; // TODO: token return
}

void lexer::_skip_whitespace_and_comments() {
  while (!_cursor.at_end()) {
    const char c = _cursor.peek();

    if (c == '#') {
      _cursor.advance_while([](char c) { return !is_newline(c); });
      continue;
    }

    if (!is_whitespace(c))
      return;

    _cursor.advance_while(is_whitespace);
  }
}

} // namespace orchard::manifest
