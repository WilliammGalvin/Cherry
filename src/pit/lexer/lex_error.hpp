/// @file lex_error.hpp
/// @brief Defines the lex_error struct and lex_error_kind enum for representing
///        lexical errors in the lexer.
#pragma once

#include <cstdint>
#include <format>
#include <string_view>
#include <utility>

namespace pit::lexer {

/// @brief Represents the different kinds of lexical errors that can occur
///        during the lexing process.
enum class lex_error_kind {
  unterminated_string,
  unterminated_block_comment,
  invalid_escape_sequence,
  unexpected_character,
};

/// @brief Represents a lexical error that occurred during the lexing process.
struct lex_error {
  using position_type =
      std::uint32_t; ///< Type used for representing the position of the error
                     ///< in the source code.

  lex_error_kind kind;
  position_type offset;
  position_type line;
  position_type column;
};

/// @brief Converts a lex_error_kind to its string representation.
/// @param kind The lex_error_kind to convert.
/// @return A string_view representing the string representation of the
///         lex_error_kind.
constexpr std::string_view to_str(lex_error_kind kind) noexcept {
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

/// @brief Converts a lex_error to its string representation, including the line
///        and column information.
/// @param err The lex_error to convert.
/// @return A string representing the string representation of the lex_error,
///         including the line and column information.
inline std::string to_str(const lex_error &err) {
  return std::format("{}:{}: error: {}", err.line, err.column,
                     to_str(err.kind));
}

} // namespace pit::lexer
