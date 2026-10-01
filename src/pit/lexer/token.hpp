/// @file token.hpp
/// @brief Defines the `token` struct, which represents a lexical token in the
///        lexer.
#pragma once

#include "pit/lexer/token_type.hpp"

#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>

namespace pit::lexer {

struct token {
  using position_type =
      std::uint32_t; ///< Type used for representing the position of the token
                     ///< in the source code.

  token_type type;
  std::string_view contents;
  position_type offset;
  position_type line;
  position_type column;
};

/// @brief Compares two tokens for equality.
/// @param lhs The left-hand side token.
/// @param rhs The right-hand side token.
/// @return `true` if the tokens are equal, `false` otherwise.
constexpr bool operator==(const token &lhs, const token &rhs) noexcept {
  return lhs.type == rhs.type && lhs.contents == rhs.contents &&
         lhs.offset == rhs.offset && lhs.line == rhs.line &&
         lhs.column == rhs.column;
}

/// @brief Converts a token to a human-readable string representation.
/// @param tok The token to convert.
/// @return A string representing the token in a human-readable format.
inline std::string to_str(const token &tok) {
  const std::string_view name = token_type_to_str(tok.type);

  std::string out;
  out.reserve(name.size() + tok.contents.size() + 3);
  out += name;
  out += " \"";
  out += tok.contents;
  out += '"';
  return out;
}

/// @brief Prints a token to the specified output stream in a human-readable
///        format.
inline std::ostream &operator<<(std::ostream &os, const token &tok) {
  return os << to_str(tok);
}

/// @brief Prints a token to the specified output stream in a human-readable
///        format.
/// @param tok The token to print.
/// @param os The output stream to print the token to.
///
/// @note This function is intended for use with Google Test's `PrintTo`
///       mechanism, which allows for custom printing of user-defined types in
///       test output.
inline void PrintTo(const token &tok, std::ostream *os) {
  *os << to_str(tok) << " at " << tok.line << ':' << tok.column << " (offset "
      << tok.offset << ')';
}

} // namespace pit::lexer
