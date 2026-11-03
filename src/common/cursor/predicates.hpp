#pragma once

namespace common::cursor {

constexpr bool is_digit(char c) noexcept { return c >= '0' && c <= '9'; }

constexpr bool is_hex_digit(char c) noexcept {
  return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

constexpr bool is_alpha(char c) noexcept {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

constexpr bool is_whitespace(char c) noexcept {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

constexpr bool is_ident_start(char c) noexcept {
  return is_alpha(c) || c == '_';
}

constexpr bool is_ident_continue(char c) noexcept {
  return is_ident_start(c) || is_digit(c);
}

constexpr bool is_bin_digit(char c) noexcept { return c == '0' || c == '1'; }
constexpr bool is_oct_digit(char c) noexcept { return c >= '0' && c <= '8'; }

constexpr bool is_digit_or_sep(char c) noexcept {
  return is_digit(c) || c == '_';
}

constexpr bool is_exponent(char c) noexcept { return c == 'e' || c == 'E'; }

constexpr bool is_lower(char c) noexcept { return c >= 'a' && c <= 'z'; }
constexpr bool is_upper(char c) noexcept { return c >= 'A' && c <= 'Z'; }
constexpr bool is_alnum(char c) noexcept { return is_alpha(c) || is_digit(c); }

constexpr bool is_newline(char c) noexcept { return c == '\n' || c == '\r'; }

constexpr bool is_inline_space(char c) noexcept {
  return c == ' ' || c == '\t' || c == '\v' || c == '\f';
}

constexpr bool is_quote(char c) noexcept { return c == '\'' || c == '"'; }

constexpr bool is_string_body(char c) noexcept {
  return c != '"' && c != '\\' && c != '\n' && c != '\r';
}

constexpr bool is_punct(char c) noexcept {
  return (c >= '!' && c <= '/') || (c >= ':' && c <= '@') ||
         (c >= '[' && c <= '`') || (c >= '{' && c <= '~');
}

constexpr bool is_bracket(char c) noexcept {
  return c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']';
}

constexpr bool is_operator_char(char c) noexcept {
  return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '=' ||
         c == '<' || c == '>' || c == '!' || c == '&' || c == '|' || c == '^' ||
         c == '~' || c == '?' || c == ':';
}

constexpr bool is_ascii(char c) noexcept {
  return static_cast<unsigned char>(c) < 0x80;
}

constexpr bool is_utf8_byte(char c) noexcept {
  return static_cast<unsigned char>(c) >= 0x80;
}

constexpr bool is_printable(char c) noexcept { return c >= ' ' && c <= '~'; }

constexpr bool is_control(char c) noexcept {
  return static_cast<unsigned char>(c) < 0x20 || c == 0x7F;
}

} // namespace common::cursor
