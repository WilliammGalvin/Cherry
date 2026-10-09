#include "resolve.hpp"
#include "node_layout.hpp"

#include <charconv>
#include <optional>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace pit::sema {

namespace {

using ast::node_id;
using ast::node_tag;
using ast::token_id;

class resolver {
public:
  resolver(sema_context &cx, const ast::tree &tree, file_id file, def_id module,
           module_scope &scope, const token_text_fn &text)
      : _cx(cx), _tree(tree), _file(file), _module(module), _scope(scope),
        _text(text) {}

  void run() {
    const auto end = static_cast<std::uint32_t>(_cx.defs.end_id());
    for (std::uint32_t i = 1; i < end; ++i) {
      const auto id = static_cast<def_id>(i);
      const def &d = _cx.defs.get(id);
      if (d.parent != _module || d.file != _file)
        continue;

      switch (d.kind) {
      case def_kind::struct_:
        _ensure_struct_fields(id);
        break;

      case def_kind::fn:
      case def_kind::extern_fn:
        _resolve_fn(id);
        break;

      case def_kind::type_alias:
        _resolve_alias(id);
        break;

      default:
        break;
      }
    }
  }

private:
  sema_context &_cx;
  const ast::tree &_tree;
  file_id _file;
  def_id _module;
  module_scope &_scope;
  const token_text_fn &_text;

  std::vector<std::pair<name_id, def_id>> _generics;

  struct generics_guard {
    resolver &r;
    std::vector<std::pair<name_id, def_id>> saved;

    explicit generics_guard(resolver &rr)
        : r(rr), saved(std::move(rr._generics)) {
      r._generics.clear();
    }

    ~generics_guard() { r._generics = std::move(saved); }

    generics_guard(const generics_guard &) = delete;
    generics_guard &operator=(const generics_guard &) = delete;
  };

  static def_id _nth(def_id first, std::uint32_t i) {
    return static_cast<def_id>(static_cast<std::uint32_t>(first) + i);
  }

  name_id _name_of(token_id t) { return _cx.names.intern(_text(t)); }

  std::string_view _text_of(name_id n) const { return _cx.names.text(n); }

  type_id _fail(token_id at, std::string_view message) {
    _cx.diags.report(severity::error, _file, at, message);
    return type_id::error;
  }

  std::optional<def_id> _lookup(name_id n) const {
    for (auto it = _generics.rbegin(); it != _generics.rend(); ++it)
      if (it->first == n)
        return it->second;

    return _scope.lookup(n);
  }

  template <typename Accept>
  void _make_children(def_id parent, def_kind kind, ast::id_view<node_id> nodes,
                      Accept accept, def_id &first, std::uint32_t &count) {
    first = def_id::none;
    count = 0;
    for (node_id n : nodes) {
      if (!accept(n))
        continue;

      def d;
      d.kind = kind;
      d.name_token = _tree.main_token(n);
      d.name = _name_of(d.name_token);
      d.file = _file;
      d.decl = n;
      d.parent = parent;
      d.index = count;
      d.is_public = (_tree.flags(n) & ast::flag::is_public) != 0;

      const def_id id = _cx.defs.add(d);
      if (count == 0)
        first = id;

      ++count;
    }
  }

  void _report_duplicates(def_id first, std::uint32_t count,
                          std::string_view what) {
    for (std::uint32_t i = 1; i < count; ++i) {
      const def &later = _cx.defs.get(_nth(first, i));
      for (std::uint32_t j = 0; j < i; ++j) {
        const def &earlier = _cx.defs.get(_nth(first, j));

        if (earlier.name != later.name)
          continue;

        _cx.diags.error(_file, later.name_token, "duplicate {} '{}'", what,
                        _text_of(later.name));
        _cx.diags.note(_file, earlier.name_token, "first declared here");
        break;
      }
    }
  }

  void _make_generics(def_id owner, ast::sub_range range) {
    def_id first;
    std::uint32_t count;
    _make_children(
        owner, def_kind::generic_param, _tree.range<node_id>(range),
        [&](node_id n) { return _tree.tag(n) == node_tag::generic_param; },
        first, count);

    for (std::uint32_t i = 0; i < count; ++i)
      _cx.defs.get(_nth(first, i)).type = _cx.types.generic_param(i);
    _report_duplicates(first, count, "generic parameter");

    def &o = _cx.defs.get(owner);
    o.first_generic = first;
    o.generic_count = count;
  }

  void _enter_generics(def_id owner) {
    const def &o = _cx.defs.get(owner);
    for (const def &g : _cx.defs.slice(o.first_generic, o.generic_count))
      _generics.emplace_back(g.name, _nth(o.first_generic, g.index));
  }

  std::optional<std::uint32_t> _array_length(node_id n) {
    if (_tree.tag(n) != node_tag::int_literal)
      return std::nullopt;

    std::string digits;
    for (char c : _text(_tree.main_token(n)))
      if (c != '_')
        digits += c;

    int base = 10;
    const char *begin = digits.data();
    const char *end = begin + digits.size();
    if (digits.size() > 2 && digits[0] == '0' &&
        (digits[1] == 'x' || digits[1] == 'X')) {

      base = 16;
      begin += 2;
    }

    std::uint32_t value = 0;
    const auto r = std::from_chars(begin, end, value, base);
    if (r.ec != std::errc{} || r.ptr != end)
      return std::nullopt;

    return value;
  }

  type_id _resolve_type(node_id n) {
    const token_id tok = _tree.main_token(n);

    switch (_tree.tag(n)) {
    case node_tag::type_name:
      return _resolve_name(tok);

    case node_tag::type_pointer:
      return _cx.types.pointer_to(_resolve_type(layout::inner_type(_tree, n)));

    case node_tag::type_optional:
      return _cx.types.optional_of(_resolve_type(layout::inner_type(_tree, n)));

    case node_tag::type_error_union:
      return _cx.types.error_union_of(
          _resolve_type(layout::inner_type(_tree, n)));

    case node_tag::type_array: {
      const type_id elem = _resolve_type(layout::array_elem(_tree, n));
      const auto len = _array_length(layout::array_len(_tree, n));
      if (!len)
        return _fail(tok, "array length must be an integer literal");

      return _cx.types.array_of(elem, *len);
    }

    case node_tag::type_generic:
      return _resolve_generic(n);

    case node_tag::type_path:
      return _fail(tok, "qualified type paths are not supported yet");

    default:
      return _fail(tok, "expected a type");
    }
  }

  type_id _resolve_name(token_id tok) {
    const name_id name = _name_of(tok);

    const auto found = _lookup(name);
    if (!found) {
      const type_id builtin = _cx.builtin_type(name);
      if (builtin != type_id::invalid)
        return builtin;

      _cx.diags.error(_file, tok, "unknown type '{}'", _text_of(name));
      return type_id::error;
    }

    switch (_cx.defs.get(*found).kind) {
    case def_kind::generic_param:
      return _cx.defs.get(*found).type;

    case def_kind::struct_: {
      const type_id t = _struct_header(*found);
      const std::uint32_t expected = _cx.defs.get(*found).generic_count;
      if (expected != 0) {
        _cx.diags.error(_file, tok,
                        "'{}' expects {} generic argument(s), found 0",
                        _text_of(name), expected);
        return type_id::error;
      }

      return t;
    }

    case def_kind::type_alias:
      return _resolve_alias(*found);

    default:
      _cx.diags.error(_file, tok, "'{}' is not a type", _text_of(name));
      return type_id::error;
    }
  }

  type_id _resolve_generic(node_id n) {
    const token_id tok = _tree.main_token(n);
    const node_id base = layout::generic_base(_tree, n);

    if (_tree.tag(base) != node_tag::type_name)
      return _fail(tok, "generic arguments require a named type");

    const token_id base_tok = _tree.main_token(base);
    const name_id name = _name_of(base_tok);
    const auto found = _lookup(name);

    if (!found) {
      _cx.diags.error(_file, base_tok, "unknown type '{}'", _text_of(name));
      return type_id::error;
    }

    if (_cx.defs.get(*found).kind != def_kind::struct_) {
      _cx.diags.error(_file, base_tok, "'{}' is not a generic type",
                      _text_of(name));
      return type_id::error;
    }

    _struct_header(*found);

    std::vector<type_id> args;
    for (node_id a : layout::generic_args(_tree, n))
      args.push_back(_resolve_type(a));

    const std::uint32_t expected = _cx.defs.get(*found).generic_count;
    if (args.size() != expected) {
      _cx.diags.error(_file, base_tok,
                      "'{}' expects {} generic argument(s), found {}",
                      _text_of(name), expected, args.size());

      return type_id::error;
    }

    return _cx.types.struct_type(*found, args);
  }

  type_id _resolve_alias(def_id d) {
    {
      def &df = _cx.defs.get(d);
      if (df.state == def_state::resolved)
        return df.type;

      if (df.state == def_state::resolving) {
        _cx.diags.error(_file, df.name_token,
                        "type alias '{}' refers to itself", _text_of(df.name));
        return type_id::error;
      }

      df.state = def_state::resolving;
    }

    generics_guard guard(*this);
    const node_id decl = _cx.defs.get(d).decl;
    const type_id target = _resolve_type(layout::alias_target(_tree, decl));

    def &df = _cx.defs.get(d);
    df.type = target;
    df.state = def_state::resolved;
    return target;
  }

  type_id _struct_header(def_id d) {
    if (_cx.defs.get(d).type != type_id::invalid)
      return _cx.defs.get(d).type;

    const node_id decl = _cx.defs.get(d).decl;
    _make_generics(d, layout::struct_ext(_tree, decl).generic_params);

    std::vector<type_id> params;
    const def &df = _cx.defs.get(d);
    for (std::uint32_t i = 0; i < df.generic_count; ++i)
      params.push_back(_cx.types.generic_param(i));

    const type_id t = _cx.types.struct_type(d, params);
    _cx.defs.get(d).type = t;
    return t;
  }

  void _ensure_struct_fields(def_id d) {
    _struct_header(d);
    if (_cx.defs.get(d).state != def_state::unresolved)
      return;

    _cx.defs.get(d).state = def_state::resolving;

    generics_guard guard(*this);
    _enter_generics(d);

    const node_id decl = _cx.defs.get(d).decl;
    const ast::struct_extra se = layout::struct_ext(_tree, decl);

    def_id first;
    std::uint32_t count;
    _make_children(
        d, def_kind::field, _tree.range<node_id>(se.fields),
        [&](node_id n) { return _tree.tag(n) == node_tag::field_decl; }, first,
        count);
    {
      def &sd = _cx.defs.get(d);
      sd.first_child = first;
      sd.child_count = count;
    }
    _report_duplicates(first, count, "field");

    for (std::uint32_t i = 0; i < count; ++i) {
      const def_id fid = _nth(first, i);
      const node_id fnode = _cx.defs.get(fid).decl;
      const type_id t = _resolve_type(layout::member_type(_tree, fnode));
      const token_id at = _cx.defs.get(fid).name_token;

      _cx.defs.get(fid).type = t;
      _require_complete(t, at);
      _cx.defs.get(fid).state = def_state::resolved;
    }

    _cx.defs.get(d).state = def_state::resolved;
  }

  void _require_complete(type_id t, token_id at) {
    while (_cx.types.is_compound(t)) {
      const type_info &ti = _cx.types.info(t);
      switch (ti.kind) {
      case type_kind::array:
      case type_kind::optional:
      case type_kind::error_union:
        t = ti.elem;
        continue;

      case type_kind::struct_: {
        const def_id sd = ti.def;
        switch (_cx.defs.get(sd).state) {
        case def_state::resolving:
          _cx.diags.error(_file, at, "struct '{}' contains itself by value",
                          _text_of(_cx.defs.get(sd).name));
          break;

        case def_state::unresolved:
          _ensure_struct_fields(sd);
          break;

        case def_state::resolved:
          break;
        }

        return;
      }

      default:
        return;
      }
    }
  }

  void _resolve_fn(def_id d) {
    if (_cx.defs.get(d).state != def_state::unresolved)
      return;

    _cx.defs.get(d).state = def_state::resolving;

    generics_guard guard(*this);

    const node_id decl = _cx.defs.get(d).decl;
    const ast::fn_extra fe = layout::fn_ext(_tree, decl);

    _make_generics(d, fe.generic_params);
    _enter_generics(d);

    for (node_id p : _tree.range<node_id>(fe.params)) {
      if (_tree.tag(p) == node_tag::self_param)
        _cx.diags.error(_file, _tree.main_token(p),
                        "'self' parameter outside of an impl");
    }

    def_id first;
    std::uint32_t count;
    _make_children(
        d, def_kind::param, _tree.range<node_id>(fe.params),
        [&](node_id n) { return _tree.tag(n) == node_tag::param; }, first,
        count);
    {
      def &fd = _cx.defs.get(d);
      fd.first_child = first;
      fd.child_count = count;
    }
    _report_duplicates(first, count, "parameter");

    std::vector<type_id> param_types;
    for (std::uint32_t i = 0; i < count; ++i) {
      const def_id pid = _nth(first, i);
      const node_id pnode = _cx.defs.get(pid).decl;
      const type_id t = _resolve_type(layout::member_type(_tree, pnode));
      const token_id at = _cx.defs.get(pid).name_token;

      _cx.defs.get(pid).type = t;
      _cx.defs.get(pid).state = def_state::resolved;
      _require_complete(t, at);
      param_types.push_back(t);
    }

    type_id ret = type_id::void_;
    if (fe.return_type != node_id::none) {
      ret = _resolve_type(fe.return_type);
      _require_complete(ret, _tree.main_token(fe.return_type));
    }

    def &fd = _cx.defs.get(d);
    fd.type = _cx.types.fn_type(param_types, ret);
    fd.state = def_state::resolved;
  }
};

} // namespace

void resolve_signatures(sema_context &cx, const ast::tree &tree, file_id file,
                        def_id module, module_scope &scope,
                        const token_text_fn &token_text) {
  resolver(cx, tree, file, module, scope, token_text).run();
}

} // namespace pit::sema
