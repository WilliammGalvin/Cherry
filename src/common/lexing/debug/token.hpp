#pragma once

#include "common/debug/lexing/token_type.hpp"
#include "common/lexing/token.hpp"

#include <ostream>
#include <string>
#include <string_view>

namespace common::debug::lexing {

using namespace common::lexing;

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

} // namespace common::debug::lexing
