#pragma once

#include <cstdint>

namespace common::lexing {

enum class lex_error_kind {
  unterminated_string,
  unterminated_block_comment,
  invalid_escape_sequence,
  unexpected_character,
};

struct lex_error {
  using position_type = std::uint32_t;

  lex_error_kind kind;
  position_type offset;
  position_type line;
  position_type column;
};

} // namespace common::lexing
