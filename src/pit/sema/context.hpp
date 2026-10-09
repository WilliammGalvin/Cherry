#pragma once

#include "arena.hpp"
#include "def.hpp"
#include "diagnostic.hpp"
#include "intern_table.hpp"
#include "name.hpp"
#include "type.hpp"

#include <string>
#include <unordered_map>

namespace pit::sema {

class sema_context {
public:
  explicit sema_context(std::size_t arena_block_bytes = 64 * 1024);

  sema_context(const sema_context &) = delete;
  sema_context &operator=(const sema_context &) = delete;

  sema_context(sema_context &&) = delete;
  sema_context &operator=(sema_context &&) = delete;

  arena memory;
  intern_table names;
  type_table types;
  def_table defs;
  diagnostic_sink diags;

  type_id builtin_type(name_id n) const;

  std::string type_name(type_id t) const;

  type_table::def_namer def_namer() const;

private:
  std::unordered_map<name_id, type_id> _builtin_types;
};

} // namespace pit::sema
