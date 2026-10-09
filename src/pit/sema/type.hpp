#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

namespace pit::sema {

enum class def_id : std::uint32_t { none = 0 };

enum class type_id : std::uint32_t {
  invalid = 0,
  error,

  void_,
  bool_,

  i8,
  i16,
  i32,
  i64,
  i128,
  isize,

  u8,
  u16,
  u32,
  u64,
  u128,
  usize,

  f32,
  f64,

  first_compound,
};

constexpr std::uint32_t to_index(type_id t) {
  return static_cast<std::uint32_t>(t);
}

constexpr bool is_signed_int(type_id t) {
  return t >= type_id::i8 && t <= type_id::isize;
}

constexpr bool is_unsigned_int(type_id t) {
  return t >= type_id::u8 && t <= type_id::usize;
}

constexpr bool is_integer(type_id t) {
  return is_signed_int(t) || is_unsigned_int(t);
}

constexpr bool is_float(type_id t) {
  return t == type_id::f32 || t == type_id::f64;
}

constexpr bool is_numeric(type_id t) { return is_integer(t) || is_float(t); }

constexpr bool is_primitive(type_id t) { return t < type_id::first_compound; }

constexpr bool is_error(type_id t) { return t == type_id::error; }

enum class type_kind : std::uint8_t {
  pointer,
  array,
  optional,
  error_union,
  fn,
  struct_,
  generic_param,
};

struct type_info {
  type_kind kind{};
  type_id elem{type_id::invalid};
  std::uint32_t length{0};
  def_id def{def_id::none};
  std::uint32_t args_start{0};
  std::uint32_t args_count{0};
};

class type_table {
public:
  type_table() = default;

  type_table(const type_table &) = delete;
  type_table &operator=(const type_table &) = delete;

  type_id pointer_to(type_id pointee);
  type_id array_of(type_id element, std::uint32_t length);
  type_id optional_of(type_id inner);
  type_id error_union_of(type_id success);
  type_id fn_type(std::span<const type_id> params, type_id ret);
  type_id struct_type(def_id def, std::span<const type_id> args = {});
  type_id generic_param(std::uint32_t index);

  bool is_compound(type_id t) const;
  type_kind kind_of(type_id t) const;
  const type_info &info(type_id t) const;
  std::span<const type_id> args(type_id t) const;

  type_id elem_of(type_id t) const;

  bool is_pointer(type_id t) const;
  bool is_array(type_id t) const;
  bool is_optional(type_id t) const;
  bool is_error_union(type_id t) const;
  bool is_fn(type_id t) const;
  bool is_struct(type_id t) const;
  bool is_generic_param(type_id t) const;

  bool coerces(type_id from, type_id to) const;

  using def_namer = std::function<std::string(def_id)>;

  std::string display_name(type_id t, const def_namer &def_name = {}) const;

  std::size_t compound_count() const { return _infos.size(); }

private:
  std::vector<type_info> _infos;
  std::vector<type_id> _arg_pool;
  std::unordered_map<std::uint64_t, std::vector<type_id>> _buckets;

  type_id _intern(const type_info &proto, std::span<const type_id> args);
  std::size_t _index_of(type_id t) const;
};

} // namespace pit::sema
