#pragma once

#include "node_tag.hpp"

#include <cstdint>

namespace pit::ast {

using id_type = std::uint32_t;

enum class node_id : id_type { root = 0, none = 0 };

enum class token_id : id_type { none = 0xFFFFFFFF };

enum class extra_id : id_type {};

struct sub_range {
  extra_id start;
  extra_id end;
};

struct node_data {
  id_type lhs;
  id_type rhs;
};

struct node {
  node_tag tag;
  token_id main_token;
  node_data data;
};

using node_flags = std::uint8_t;

namespace flag {
inline constexpr node_flags is_public = 1u << 0;
}

} // namespace pit::ast
