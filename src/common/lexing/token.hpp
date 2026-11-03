#pragma once

#include <common/io/source_manager.hpp>
#include <cstdint>

namespace common::lexing {

using base_token_type = std::uint8_t;

struct token {
  io::source_id id;
  base_token_type type;
  std::uint32_t offset;
  std::uint32_t length;
};

} // namespace common::lexing
