#pragma once

#include "name.hpp"
#include "pit/ast/node.hpp"
#include "type.hpp"

#include <cstdint>

namespace pit::sema {

enum class file_id : std::uint32_t;

enum class def_kind : std::uint8_t {
  fn,
  extern_fn,
  struct_,
  field,
  const_,
  static_,
  type_alias,
  error_name,
  param,
  local,
  generic_param,
  module,
};

enum class def_state : std::uint8_t {
  unresolved,
  resolving,
  resolved,
};

struct def {
  def_kind kind;
  def_state state{def_state::unresolved};
  bool is_public{false};

  name_id name{name_id::none};
  ast::token_id name_token{ast::token_id::none};

  file_id file{};
  ast::node_id decl{ast::node_id::none};

  type_id type{type_id::invalid};
  def_id parent{def_id::none};
  std::uint32_t index{0};
};

} // namespace pit::sema
