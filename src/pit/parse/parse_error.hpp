#pragma once

#include <cstdint>
#include <sstream>
#include <string>

namespace pit::parse {

struct parse_error {
  using position_type = std::uint32_t;

  std::string message;
  position_type offset;
  position_type line;
  position_type column;
};

inline std::string to_str(const parse_error &err) {
  std::ostringstream out;
  out << err.line << ':' << err.column << ": error: " << err.message;
  return out.str();
}

} // namespace pit::parse
