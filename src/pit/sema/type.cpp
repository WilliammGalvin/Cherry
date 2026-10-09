#include "type.hpp"

#include <algorithm>
#include <cassert>

namespace pit::sema {

namespace {

constexpr std::uint64_t mix(std::uint64_t h, std::uint64_t v) {
  return h ^ (v + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2));
}

std::uint64_t hash_info(const type_info &i, std::span<const type_id> args) {
  std::uint64_t h = static_cast<std::uint64_t>(i.kind);
  h = mix(h, to_index(i.elem));
  h = mix(h, i.length);
  h = mix(h, static_cast<std::uint32_t>(i.def));

  for (type_id a : args)
    h = mix(h, to_index(a));

  return h;
}

bool any_error(std::span<const type_id> ts) {
  return std::any_of(ts.begin(), ts.end(),
                     [](type_id t) { return is_error(t); });
}

unsigned fixed_width(type_id t) {
  switch (t) {
  case type_id::i8:
  case type_id::u8:
    return 8;

  case type_id::i16:
  case type_id::u16:
    return 16;

  case type_id::i32:
  case type_id::u32:
    return 32;

  case type_id::i64:
  case type_id::u64:
    return 64;

  case type_id::i128:
  case type_id::u128:
    return 128;

  default:
    return 0;
  }
}

const char *primitive_name(type_id t) {
  switch (t) {
  case type_id::invalid:
    return "<invalid>";
  case type_id::error:
    return "<error>";
  case type_id::void_:
    return "void";
  case type_id::bool_:
    return "bool";
  case type_id::i8:
    return "i8";
  case type_id::i16:
    return "i16";
  case type_id::i32:
    return "i32";
  case type_id::i64:
    return "i64";
  case type_id::i128:
    return "i128";
  case type_id::isize:
    return "isize";
  case type_id::u8:
    return "u8";
  case type_id::u16:
    return "u16";
  case type_id::u32:
    return "u32";
  case type_id::u64:
    return "u64";
  case type_id::u128:
    return "u128";
  case type_id::usize:
    return "usize";
  case type_id::f32:
    return "f32";
  case type_id::f64:
    return "f64";

  default:
    return "<?>";
  }
}

} // namespace

std::size_t type_table::_index_of(type_id t) const {
  assert(is_compound(t));
  return to_index(t) - to_index(type_id::first_compound);
}

type_id type_table::_intern(const type_info &proto,
                            std::span<const type_id> args) {
  const std::uint64_t h = hash_info(proto, args);
  auto &bucket = _buckets[h];

  for (type_id cand : bucket) {
    const type_info &c = _infos[_index_of(cand)];
    if (c.kind != proto.kind || c.elem != proto.elem ||
        c.length != proto.length || c.def != proto.def ||
        c.args_count != args.size())
      continue;

    if (std::equal(args.begin(), args.end(), _arg_pool.begin() + c.args_start))
      return cand;
  }

  const std::vector<type_id> owned(args.begin(), args.end());

  type_info stored = proto;
  stored.args_start = static_cast<std::uint32_t>(_arg_pool.size());
  stored.args_count = static_cast<std::uint32_t>(owned.size());
  _arg_pool.insert(_arg_pool.end(), owned.begin(), owned.end());

  const auto id =
      static_cast<type_id>(to_index(type_id::first_compound) + _infos.size());
  _infos.push_back(stored);
  bucket.push_back(id);
  return id;
}

type_id type_table::pointer_to(type_id pointee) {
  if (is_error(pointee))
    return type_id::error;

  type_info i;
  i.kind = type_kind::pointer;
  i.elem = pointee;
  return _intern(i, {});
}

type_id type_table::array_of(type_id element, std::uint32_t length) {
  if (is_error(element))
    return type_id::error;

  type_info i;
  i.kind = type_kind::array;
  i.elem = element;
  i.length = length;
  return _intern(i, {});
}

type_id type_table::optional_of(type_id inner) {
  if (is_error(inner))
    return type_id::error;

  type_info i;
  i.kind = type_kind::optional;
  i.elem = inner;
  return _intern(i, {});
}

type_id type_table::error_union_of(type_id success) {
  if (is_error(success))
    return type_id::error;

  type_info i;
  i.kind = type_kind::error_union;
  i.elem = success;
  return _intern(i, {});
}

type_id type_table::fn_type(std::span<const type_id> params, type_id ret) {
  if (is_error(ret) || any_error(params))
    return type_id::error;

  type_info i;
  i.kind = type_kind::fn;
  i.elem = ret;
  return _intern(i, params);
}

type_id type_table::struct_type(def_id def, std::span<const type_id> args) {
  assert(def != def_id::none);
  if (any_error(args))
    return type_id::error;

  type_info i;
  i.kind = type_kind::struct_;
  i.def = def;
  return _intern(i, args);
}

type_id type_table::generic_param(std::uint32_t index) {
  type_info i;
  i.kind = type_kind::generic_param;
  i.length = index;
  return _intern(i, {});
}

bool type_table::is_compound(type_id t) const {
  return t >= type_id::first_compound &&
         to_index(t) - to_index(type_id::first_compound) < _infos.size();
}

const type_info &type_table::info(type_id t) const {
  return _infos[_index_of(t)];
}

type_kind type_table::kind_of(type_id t) const { return info(t).kind; }

std::span<const type_id> type_table::args(type_id t) const {
  const type_info &i = info(t);
  return {_arg_pool.data() + i.args_start, i.args_count};
}

type_id type_table::elem_of(type_id t) const { return info(t).elem; }

#define PIT_KIND_PRED(fn, k)                                                   \
  bool type_table::fn(type_id t) const {                                       \
    return is_compound(t) && info(t).kind == type_kind::k;                     \
  }
PIT_KIND_PRED(is_pointer, pointer)
PIT_KIND_PRED(is_array, array)
PIT_KIND_PRED(is_optional, optional)
PIT_KIND_PRED(is_error_union, error_union)
PIT_KIND_PRED(is_fn, fn)
PIT_KIND_PRED(is_struct, struct_)
PIT_KIND_PRED(is_generic_param, generic_param)
#undef PIT_KIND_PRED

bool type_table::coerces(type_id from, type_id to) const {
  if (from == to)
    return true;

  if (is_error(from) || is_error(to))
    return true;

  if (from == type_id::invalid || to == type_id::invalid)
    return false;

  const unsigned wf = fixed_width(from), wt = fixed_width(to);
  if (wf != 0 && wt != 0) {
    if (is_signed_int(from) && is_signed_int(to))
      return wf < wt;

    if (is_unsigned_int(from) && is_unsigned_int(to))
      return wf < wt;

    if (is_unsigned_int(from) && is_signed_int(to))
      return wf < wt;

    return false;
  }

  if (from == type_id::f32 && to == type_id::f64)
    return true;

  if (is_compound(to)) {
    const type_info &ti = info(to);
    if (ti.kind == type_kind::optional || ti.kind == type_kind::error_union)
      return coerces(from, ti.elem);
  }

  return false;
}

std::string type_table::display_name(type_id t,
                                     const def_namer &def_name) const {
  if (is_primitive(t))
    return primitive_name(t);

  if (!is_compound(t))
    return "<?>";

  const type_info &i = info(t);
  switch (i.kind) {
  case type_kind::pointer:
    return "*" + display_name(i.elem, def_name);

  case type_kind::array:
    return "[" + std::to_string(i.length) + "]" +
           display_name(i.elem, def_name);

  case type_kind::optional:
    return "?" + display_name(i.elem, def_name);

  case type_kind::error_union:
    return "!" + display_name(i.elem, def_name);

  case type_kind::fn: {
    std::string s = "fn(";
    bool first = true;
    for (type_id p : args(t)) {
      if (!first)
        s += ", ";

      first = false;
      s += display_name(p, def_name);
    }

    s += ") " + display_name(i.elem, def_name);
    return s;
  }

  case type_kind::struct_: {
    std::string s =
        def_name
            ? def_name(i.def)
            : "struct#" + std::to_string(static_cast<std::uint32_t>(i.def));

    if (i.args_count != 0) {
      s += "<";
      bool first = true;
      for (type_id a : args(t)) {
        if (!first)
          s += ", ";

        first = false;
        s += display_name(a, def_name);
      }

      s += ">";
    }

    return s;
  }

  case type_kind::generic_param:
    return "$" + std::to_string(i.length);
  }

  return "<?>";
}

} // namespace pit::sema
