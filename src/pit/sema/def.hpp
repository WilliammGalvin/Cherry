#pragma once

#include "name.hpp"
#include "pit/ast/node.hpp"
#include "pit/sema/file_id.hpp"
#include "type.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace pit::sema {

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
  def_kind kind{def_kind::module};
  def_state state{def_state::unresolved};
  bool is_public{false};

  name_id name{name_id::none};
  ast::token_id name_token{ast::token_id::none};

  file_id file{file_id::none};
  ast::node_id decl{ast::node_id::none};

  type_id type{type_id::invalid};
  def_id parent{def_id::none};
  std::uint32_t index{0};
};

class def_table {
public:
  def_table() {
    _defs.emplace_back(); // id = 0 reserved as none
  }

  def_table(const def_table &) = delete;
  def_table &operator=(const def_table &) = delete;

  def_id add(const def &d) {
    const auto id = static_cast<def_id>(_defs.size());
    _defs.push_back(d);
    return id;
  }

  def &get(def_id id) {
    assert(valid(id));
    return _defs[static_cast<std::size_t>(id)];
  }

  const def &get(def_id id) const {
    assert(valid(id));
    return _defs[static_cast<std::size_t>(id)];
  }

  bool valid(def_id id) const {
    const auto i = static_cast<std::size_t>(id);
    return i != 0 && i < _defs.size();
  }

  std::size_t size() const { return _defs.size() - 1; }

  static constexpr def_id first_id() { return static_cast<def_id>(1); }
  def_id end_id() const { return static_cast<def_id>(_defs.size()); }

private:
  std::vector<def> _defs;
};

} // namespace pit::sema
