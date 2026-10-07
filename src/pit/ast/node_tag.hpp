#pragma once

#include <cstdint>

namespace pit::ast {

using tag_type = std::uint32_t;

enum class node_tag : tag_type {
  root,

  fn_decl,
  extern_fn,
  param,
  self_param,
  generic_param,
  struct_decl,
  field_decl,
  impl_decl,
  static_decl,
  type_alias,
  error_decl,
  import,

  type_name,
  type_path,
  type_generic,
  type_pointer,
  type_array,
  type_optional,
  type_error_union,

  block,
  const_decl,
  var_decl,
  assign,
  assign_add,
  assign_sub,
  assign_mul,
  assign_div,
  assign_rem,
  if_simple,
  if_full,
  while_stmt,
  defer_stmt,
  errdefer_stmt,
  expr_stmt,

  return_stmt,
  break_stmt,
  continue_stmt,

  int_literal,
  float_literal,
  string_literal,
  char_literal,
  bool_literal,
  none_literal,
  identifier,
  error_value,

  call,
  type_args,
  field_access,
  index,
  struct_literal,
  field_init,
  cast,
  directive,

  neg,
  logical_not,
  address_of,
  deref,
  try_expr,
  unwrap,

  add,
  sub,
  mul,
  div,
  rem,
  eq,
  ne,
  lt,
  le,
  gt,
  ge,
  logical_and,
  logical_or,
  coalesce,

  catch_expr,

  invalid,
};

constexpr bool in_range(node_tag t, node_tag first, node_tag last) {
  return static_cast<tag_type>(t) >= static_cast<tag_type>(first) &&
         static_cast<tag_type>(t) <= static_cast<tag_type>(last);
}

constexpr bool is_type(node_tag t) {
  return in_range(t, node_tag::type_name, node_tag::type_error_union);
}

constexpr bool is_assign(node_tag t) {
  return in_range(t, node_tag::assign, node_tag::assign_rem);
}

constexpr bool is_unary(node_tag t) {
  return in_range(t, node_tag::neg, node_tag::unwrap);
}

constexpr bool is_binary(node_tag t) {
  return in_range(t, node_tag::add, node_tag::coalesce);
}

} // namespace pit::ast
