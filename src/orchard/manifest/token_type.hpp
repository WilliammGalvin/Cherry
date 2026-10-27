#pragma once

#include <cstdint>

namespace orchard::manifest {

enum class token_type : std::uint8_t {
  identifier,
  integer_literal,
  float_literal,
  string_literal,
  boolean_literal_true,
  boolean_literal_false,

  semi_colon,
  colon,
  comma,
  left_paren,
  right_paren,
  left_bracket,
  right_bracket,

  invalid,
  eof,
};

} // namespace orchard::manifest
