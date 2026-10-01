/// @file char_class.hpp
/// @brief Provides character classification functions for the lexer.
#pragma once

namespace pit::lexer::char_class {

/// @brief Checks if a character is a digit (0-9).
/// @param c The character to check.
/// @return true if the character is a digit, false otherwise.
constexpr bool is_digit(char c) noexcept { return c >= '0' && c <= '9'; }

/// @brief Checks if a character is a hexadecimal digit (0-9, a-f, A-F).
/// @param c The character to check.
/// @return true if the character is a hexadecimal digit, false otherwise.
constexpr bool is_hex_digit(char c) noexcept {
  return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

/// @brief Checks if a character is an alphabetic character (a-z or A-Z).
/// @param c The character to check.
/// @return true if the character is an alphabetic character, false otherwise.
constexpr bool is_alpha(char c) noexcept {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/// @brief Checks if a character is a whitespace character (space, tab, newline,
///        or carriage return).
/// @param c The character to check.
/// @return true if the character is a whitespace character, false otherwise.
constexpr bool is_whitespace(char c) noexcept {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

/// @brief Checks if a character is a valid identifier start character.
/// @param c The character to check.
/// @return true if the character is a valid identifier start character,
///         false otherwise.
constexpr bool is_ident_start(char c) noexcept {
  return is_alpha(c) || c == '_';
}

/// @brief Checks if a character is a valid identifier continuation character.
/// @param c The character to check.
/// @return true if the character is a valid identifier continuation character,
///         false otherwise.
constexpr bool is_ident_continue(char c) noexcept {
  return is_ident_start(c) || is_digit(c);
}

} // namespace pit::lexer::char_class
