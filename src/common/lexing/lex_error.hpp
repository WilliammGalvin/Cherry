/// @file lex_error.hpp
/// @brief Defines the lex_error struct and lex_error_kind enum for representing
///        lexical errors in the lexer.
#pragma once

#include <cstdint>
#include <format>
#include <string_view>
#include <utility>

namespace common::lexing {

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

} // namespace common::lexing
