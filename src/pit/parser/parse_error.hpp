/// @file parse_error.hpp
/// @brief Defines the parse_error struct for representing parsing errors in the
///        parser.
#pragma once

#include <cstdint>
#include <sstream>
#include <string>

namespace pit::parser {

/// @brief Represents a parsing error that occurred during the parsing process.
struct parse_error {
  using position_type =
      std::uint32_t; ///< Type used for representing the position of the error
                     ///< in the source code.

  std::string message;
  position_type offset;
  position_type line;
  position_type column;
};

/// @brief Converts a parse_error to its string representation, including the
///        line and column information.
/// @param err The parse_error to convert.
/// @return A string representing the string representation of the parse_error,
///         including the line and column information.
inline std::string to_str(const parse_error &err) {
  std::ostringstream out;
  out << err.line << ':' << err.column << ": error: " << err.message;
  return out.str();
}

} // namespace pit::parser
