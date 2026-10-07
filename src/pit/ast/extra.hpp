#pragma once

#include "node.hpp"

#include <type_traits>

namespace pit::ast {

struct fn_extra {
  sub_range generic_params;
  sub_range params;
  node_id return_type;
  sub_range attributes;
};

struct struct_extra {
  sub_range generic_params;
  sub_range fields;
  sub_range attributes;
};

struct impl_extra {
  sub_range generic_params;
  node_id target_type;
};

struct import_extra {
  sub_range path;
  sub_range selected;
};

struct if_extra {
  token_id capture;
  node_id then_block;
  node_id else_block;
};

struct catch_extra {
  token_id capture;
  node_id handler;
};

static_assert(std::is_trivially_copyable_v<fn_extra> &&
              sizeof(fn_extra) % 4 == 0);
static_assert(std::is_trivially_copyable_v<struct_extra> &&
              sizeof(struct_extra) % 4 == 0);
static_assert(std::is_trivially_copyable_v<impl_extra> &&
              sizeof(impl_extra) % 4 == 0);
static_assert(std::is_trivially_copyable_v<import_extra> &&
              sizeof(import_extra) % 4 == 0);
static_assert(std::is_trivially_copyable_v<if_extra> &&
              sizeof(if_extra) % 4 == 0);
static_assert(std::is_trivially_copyable_v<catch_extra> &&
              sizeof(catch_extra) % 4 == 0);

} // namespace pit::ast
