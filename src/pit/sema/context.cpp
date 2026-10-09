#include "context.hpp"

#include <array>
#include <string_view>
#include <utility>

namespace pit::sema {

sema_context::sema_context(std::size_t arena_block_bytes)
    : memory(arena_block_bytes), names(memory), diags(memory) {
  static constexpr std::array<std::pair<std::string_view, type_id>, 16>
      builtins{{
          {"void", type_id::void_},
          {"bool", type_id::bool_},
          {"i8", type_id::i8},
          {"i16", type_id::i16},
          {"i32", type_id::i32},
          {"i64", type_id::i64},
          {"i128", type_id::i128},
          {"isize", type_id::isize},
          {"u8", type_id::u8},
          {"u16", type_id::u16},
          {"u32", type_id::u32},
          {"u64", type_id::u64},
          {"u128", type_id::u128},
          {"usize", type_id::usize},
          {"f32", type_id::f32},
          {"f64", type_id::f64},
      }};

  _builtin_types.reserve(builtins.size());
  for (const auto &[text, id] : builtins)
    _builtin_types.emplace(names.intern(text), id);
}

type_id sema_context::builtin_type(name_id n) const {
  auto it = _builtin_types.find(n);
  return it == _builtin_types.end() ? type_id::invalid : it->second;
}

type_table::def_namer sema_context::def_namer() const {
  return [this](def_id d) -> std::string {
    if (!defs.valid(d))
      return "<unknown>";

    return std::string(names.text(defs.get(d).name));
  };
}

std::string sema_context::type_name(type_id t) const {
  return types.display_name(t, def_namer());
}

} // namespace pit::sema
