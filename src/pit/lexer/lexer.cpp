#include "pit/lexer/lexer.hpp"
#include "pit/lexer/token_type.hpp"

#include <sstream>
#include <utility>

namespace pit::lexer {

namespace {

constexpr bool is_ident_start(char c) noexcept {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

constexpr bool is_digit(char c) noexcept { return c >= '0' && c <= '9'; }

constexpr bool is_ident_continue(char c) noexcept {
  return is_ident_start(c) || is_digit(c);
}

constexpr bool is_space(char c) noexcept {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' ||
         c == '\f';
}

} // namespace

std::string_view lex_error_kind_to_str(lex_error_kind kind) noexcept {
  switch (kind) {
  case lex_error_kind::unterminated_string:
    return "unterminated string literal";
  case lex_error_kind::unterminated_block_comment:
    return "unterminated block comment";
  case lex_error_kind::invalid_escape_sequence:
    return "invalid escape sequence";
  case lex_error_kind::unexpected_character:
    return "unexpected character";
  }
  std::unreachable();
}

std::string lex_error::to_str() const {
  std::ostringstream out;
  out << line << ':' << column << ": error: " << lex_error_kind_to_str(kind);
  return out.str();
}

char lexer::peek(std::uint32_t ahead) const noexcept {
  const std::uint32_t at = index_ + ahead;
  if (at >= source_.size())
    return '\0';
  return source_[at];
}

bool lexer::starts_with(std::string_view text) const noexcept {
  return source_.substr(index_).starts_with(text);
}

char lexer::bump() noexcept {
  const char c = source_[index_++];
  if (c == '\n') {
    ++line_;
    line_start_ = index_;
  }
  return c;
}

void lexer::advance(std::uint32_t count) noexcept {
  for (std::uint32_t i = 0; i < count && !at_end(); ++i)
    bump();
}

void lexer::push(token_type type, std::uint32_t start, std::uint32_t start_line,
                 std::uint32_t start_column) {
  tokens_.emplace_back(type, source_.substr(start, index_ - start), start,
                       start_line, start_column);
}

lex_error lexer::error_here(lex_error_kind kind) const noexcept {
  return lex_error{kind, index_, line_, column()};
}

// Skips whitespace, line comments, and nested block comments.
std::expected<void, lex_error> lexer::skip_trivia() {
  while (!at_end()) {
    if (is_space(peek())) {
      bump();
      continue;
    }

    if (starts_with("//")) {
      while (!at_end() && peek() != '\n')
        bump();
      continue;
    }

    if (starts_with("/*")) {
      const lex_error unterminated =
          error_here(lex_error_kind::unterminated_block_comment);
      advance(2);
      std::uint32_t depth = 1;
      while (depth > 0) {
        if (at_end())
          return std::unexpected{unterminated};
        if (starts_with("/*")) {
          advance(2);
          ++depth;
          continue;
        }
        if (starts_with("*/")) {
          advance(2);
          --depth;
          continue;
        }
        bump();
      }
      continue;
    }

    break;
  }
  return {};
}

void lexer::lex_identifier_or_keyword() {
  const std::uint32_t start = index_;
  const std::uint32_t start_line = line_;
  const std::uint32_t start_column = column();

  while (!at_end() && is_ident_continue(peek()))
    bump();

  const std::string_view word = source_.substr(start, index_ - start);
  const token_type type =
      keyword_from_text(word).value_or(token_type::identifier);
  push(type, start, start_line, start_column);
}

void lexer::lex_number() {
  const std::uint32_t start = index_;
  const std::uint32_t start_line = line_;
  const std::uint32_t start_column = column();
  bool is_float = false;

  while (!at_end() && is_digit(peek()))
    bump();

  // A '.' only belongs to the number when a digit follows it, so that `1.foo`
  // lexes as integer, dot, identifier rather than a malformed float.
  if (peek() == '.' && is_digit(peek(1))) {
    is_float = true;
    bump();
    while (!at_end() && is_digit(peek()))
      bump();
  }

  if (peek() == 'f' || peek() == 'F') {
    is_float = true;
    bump();
  }

  push(is_float ? token_type::float_literal : token_type::integer_literal,
       start, start_line, start_column);
}

void lexer::lex_directive() {
  const std::uint32_t start = index_;
  const std::uint32_t start_line = line_;
  const std::uint32_t start_column = column();

  bump(); // '@'
  while (!at_end() && is_ident_continue(peek()))
    bump();

  push(token_type::directive, start, start_line, start_column);
}

std::expected<void, lex_error> lexer::lex_string_literal() {
  const std::uint32_t start = index_;
  const std::uint32_t start_line = line_;
  const std::uint32_t start_column = column();
  const lex_error unterminated =
      error_here(lex_error_kind::unterminated_string);

  bump(); // opening quote

  while (true) {
    if (at_end() || peek() == '\n')
      return std::unexpected{unterminated};

    const char c = peek();

    if (c == '"') {
      bump();
      push(token_type::string_literal, start, start_line, start_column);
      return {};
    }

    if (c == '\\') {
      const lex_error bad_escape =
          error_here(lex_error_kind::invalid_escape_sequence);
      bump();
      if (at_end())
        return std::unexpected{unterminated};
      switch (peek()) {
      case 'n':
      case 't':
      case 'r':
      case '0':
      case '\\':
      case '\'':
      case '"':
        bump();
        break;
      default:
        return std::unexpected{bad_escape};
      }
      continue;
    }

    bump();
  }
}

std::expected<void, lex_error> lexer::lex_symbol() {
  const std::uint32_t start = index_;
  const std::uint32_t start_line = line_;
  const std::uint32_t start_column = column();

  // Emits a two-character token when the next character matches `second`,
  // otherwise the one-character token. This is what makes longest-match
  // correct by construction.
  const auto pick = [&](char second, token_type two, token_type one) {
    bump();
    if (peek() == second) {
      bump();
      push(two, start, start_line, start_column);
    } else {
      push(one, start, start_line, start_column);
    }
  };

  const auto single = [&](token_type type) {
    bump();
    push(type, start, start_line, start_column);
  };

  switch (peek()) {
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
    if (peek(1) == '&') {
      advance(2);
      push(token_type::logical_and, start, start_line, start_column);
      return {};
    }
    return std::unexpected{error_here(lex_error_kind::unexpected_character)};
  case '|':
    if (peek(1) == '|') {
      advance(2);
      push(token_type::logical_or, start, start_line, start_column);
      return {};
    }
    return std::unexpected{error_here(lex_error_kind::unexpected_character)};
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
    return std::unexpected{error_here(lex_error_kind::unexpected_character)};
  }
}

lex_result lexer::lex() {
  tokens_.clear();
  index_ = 0;
  line_ = 1;
  line_start_ = 0;

  while (true) {
    if (auto trivia = skip_trivia(); !trivia)
      return std::unexpected{trivia.error()};

    if (at_end())
      break;

    const char c = peek();

    if (is_ident_start(c)) {
      lex_identifier_or_keyword();
    } else if (is_digit(c)) {
      lex_number();
    } else if (c == '"') {
      if (auto result = lex_string_literal(); !result)
        return std::unexpected{result.error()};
    } else if (c == '@') {
      lex_directive();
    } else {
      if (auto result = lex_symbol(); !result)
        return std::unexpected{result.error()};
    }
  }

  tokens_.emplace_back(token_type::eof, std::string_view{}, index_, line_,
                       column());
  return std::move(tokens_);
}

} // namespace pit::lexer
