#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace pit::sema {

enum class def_id : std::uint32_t {};

enum class type_id : std::uint8_t {
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

constexpr bool is_primitive(type_id t) { return t < type_id::first_compound; }

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
  type_kind kind;
  type_id elem{type_id::invalid};
  std::uint32_t length{};
  def_id def{};
  std::uint32_t args_start{};
  std::uint32_t args_end{};
};

class type_table {
public:
  type_table();

  type_id pointer_to(type_id pointee);
  type_id array_of(type_id element, std::uint32_t length);
  type_id optional_of(type_id inner);
  type_id error_union_of(type_id success);
  type_id fn_type(const std::vector<type_id> &params, type_id ret);
  type_id struct_type(def_id def, const std::vector<type_id> &args);
  type_id generic_param(std::uint32_t index);

  type_kind kind_of(type_id t) const;
  const type_info &info(type_id t) const;
  std::vector<type_id> args(type_id t) const;

  bool is_pointer(type_id t) const;
  bool is_optional(type_id t) const;
  bool is_error_union(type_id t) const;

  std::string name(type_id t) const;

  bool coerces(type_id from, type_id to) const;

private:
  struct key;

  std::vector<type_info> _infos;
  std::vector<type_id> _arg_pool;
  std::unordered_map<std::uint64_t, std::vector<type_id>> _buckets;

  type_id _intern(const type_info &info, const std::vector<type_id> &args);
};

} // namespace pit::sema
