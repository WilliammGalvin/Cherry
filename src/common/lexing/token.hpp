/// @file token.hpp
/// @brief Defines the `token` struct, which represents a lexical token in the
///        lexer.
#pragma once

#include "common/lexing/source_file.hpp"
#include "token_type.hpp"

#include <cstdint>

namespace common::lexing {

struct token {
  source_file_id id;
  token_type type;
  std::uint32_t offset;
  std::uint32_t length;
};

} // namespace common::lexing
