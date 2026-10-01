#include "pit/lexer/lexer.hpp"
#include "pit/lexer/char_class.hpp"
#include "pit/lexer/token_type.hpp"

#include <unistd.h>

namespace pit::lexer {

std::expected<std::vector<token>, lex_error> lexer::lex() {
  _reset();

  while (true) {
    const auto trivia = _skip_comments_and_whitespace();
    if (!trivia)
      return std::unexpected{trivia.error()};

    if (_cursor.at_end())
      break;

    const auto c = _cursor.peek();

    if (char_class::is_ident_start(c)) {
      const auto result = _lex_identifier_or_keyword();
      if (!result)
        return std::unexpected{result.error()};
      continue;
    }

    if (char_class::is_digit(c)) {
      const auto result = _lex_number();
      if (!result)
        return std::unexpected{result.error()};
      continue;
    }

    if (c == '"') {
      const auto result = _lex_string_literal();
      if (!result)
        return std::unexpected{result.error()};
      continue;
    }

    if (c == '@') {
      const auto result = _lex_directive();
      if (!result)
        return std::unexpected{result.error()};
      continue;
    }

    const auto result = _lex_symbol();
    if (!result)
      return std::unexpected{result.error()};
  }

  _push_token(token_type::eof);
  return _tokens;
}

lexer::lex_result lexer::_lex_identifier_or_keyword() {
  const auto start = _cursor.offset();
  _cursor.advance(); // first character
  _cursor.advance_while(char_class::is_ident_continue);

  const auto word = _cursor.slice(start, _cursor.offset());
  const auto type = keyword_from_text(word).value_or(token_type::identifier);
  _push_token(type, start);
  return {};
}

lexer::lex_result lexer::_lex_number() {
  bool is_float = false;
  const auto start = _cursor.offset();
  _cursor.advance_while(char_class::is_digit);

  if (_cursor.peek() == '.' && _cursor.is_peek(char_class::is_digit, 1)) {
    is_float = true;
    _cursor.advance(); // decimal point
    _cursor.advance_while(char_class::is_digit);
  }

  if (_cursor.peek() == 'f' || _cursor.peek() == 'F') {
    is_float = true;
    _cursor.advance(); // float suffix
  }

  const auto num_type =
      is_float ? token_type::float_literal : token_type::integer_literal;
  _push_token(num_type, start);
  return {};
}

lexer::lex_result lexer::_lex_directive() {
  _cursor.advance(); // prefix

  const auto start = _cursor.offset();
  _cursor.advance_while(char_class::is_ident_continue);
  _push_token(token_type::directive, start);
  return {};
}

lexer::lex_result lexer::_lex_string_literal() {
  const lex_error unterminated =
      _error_here(lex_error_kind::unterminated_string);

  _cursor.advance(); // opening quote
  const auto start = _cursor.offset();

  while (true) {
    if (_cursor.at_end() || _cursor.peek() == '\n')
      return std::unexpected{unterminated};

    const char c = _cursor.peek();
    if (c == '"') {
      const auto stop = _cursor.offset();
      _cursor.advance(); // closing quote
      _push_token(token_type::string_literal, start, stop);
      return {};
    }

    if (c == '\\') {
      const lex_error bad_escape =
          _error_here(lex_error_kind::invalid_escape_sequence);

      _cursor.advance(); // backslash

      if (_cursor.at_end())
        return std::unexpected{unterminated};

      switch (_cursor.peek()) {
      case 'n':
      case 't':
      case 'r':
      case '0':
      case '\\':
      case '\'':
      case '"':
        _cursor.advance();
        break;

      default:
        return std::unexpected{bad_escape};
      }

      continue;
    }

    _cursor.advance();
  }
}

lexer::lex_result lexer::_lex_symbol() {
  const auto pick = [&](char second, token_type two, token_type one) {
    _cursor.advance();

    if (_cursor.peek() == second) {
      _cursor.advance();
      _push_token(two);
      return;
    }

    _push_token(one);
  };

  const auto single = [&](token_type type) {
    _cursor.advance();
    _push_token(type);
  };

  switch (_cursor.peek()) {
  case '+':
    pick('=', token_type::plus_equal, token_type::plus);
    return {};
  case '-':
    pick('=', token_type::minus_equal, token_type::minus);
    return {};
  case '*':
    pick('=', token_type::star_equal, token_type::star);
    return {};
  case '/':
    pick('=', token_type::slash_equal, token_type::slash);
    return {};
  case '%':
    pick('=', token_type::percent_equal, token_type::percent);
    return {};
  case '=':
    pick('=', token_type::double_equal, token_type::equal);
    return {};
  case '!':
    pick('=', token_type::bang_equal, token_type::bang);
    return {};
  case '<':
    pick('=', token_type::less_equal, token_type::less);
    return {};
  case '>':
    pick('=', token_type::greater_equal, token_type::greater);
    return {};

  case '&':
    if (_cursor.peek(1) == '&') {
      _cursor.advance(2);
      _push_token(token_type::logical_and);
      return {};
    }

    return std::unexpected{_error_here(lex_error_kind::unexpected_character)};

  case '|':
    if (_cursor.peek(1) == '|') {
      _cursor.advance(2);
      _push_token(token_type::logical_or);
      return {};
    }

    return std::unexpected{_error_here(lex_error_kind::unexpected_character)};
  case ';':
    single(token_type::semi_colon);
    return {};
  case ':':
    single(token_type::colon);
    return {};
  case ',':
    single(token_type::comma);
    return {};
  case '.':
    single(token_type::dot);
    return {};
  case '(':
    single(token_type::left_paren);
    return {};
  case ')':
    single(token_type::right_paren);
    return {};
  case '{':
    single(token_type::left_brace);
    return {};
  case '}':
    single(token_type::right_brace);
    return {};
  case '[':
    single(token_type::left_bracket);
    return {};
  case ']':
    single(token_type::right_bracket);
    return {};

  default:
    return std::unexpected{_error_here(lex_error_kind::unexpected_character)};
  }
}

lexer::lex_result lexer::_skip_comments_and_whitespace() {
  while (!_cursor.at_end()) {
    _cursor.advance_while(char_class::is_whitespace);

    if (_cursor.starts_with("//"))
      _cursor.advance_while([](char c) { return c != '\n'; });

    if (_cursor.starts_with("/*")) {
      const lex_error unterminated =
          _error_here(lex_error_kind::unterminated_block_comment);
      _cursor.advance(2);

      int depth = 1;
      while (depth > 0) {
        if (_cursor.at_end())
          return std::unexpected{unterminated};

        if (_cursor.starts_with("/*")) {
          _cursor.advance(2);
          ++depth;
          continue;
        }

        if (_cursor.starts_with("*/")) {
          _cursor.advance(2);
          --depth;
          continue;
        }

        _cursor.advance();
      }

      continue;
    }

    break;
  }

  return {};
}

} // namespace pit::lexer
