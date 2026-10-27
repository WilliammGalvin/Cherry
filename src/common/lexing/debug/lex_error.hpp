#pragma once

#include <common/lexing/lex_error.hpp>
#include <string_view>

namespace common::debug::lexing {

using namespace common::lexing;

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
} // namespace common::debug::lexing
