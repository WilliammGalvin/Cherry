#pragma once

#include "context.hpp"
#include "declare.hpp"
#include "pit/ast/tree.hpp"

#include <optional>
#include <vector>

namespace pit::sema {

inline std::optional<def_kind> top_level_def_kind(ast::node_tag tag) {
  switch (tag) {
  case ast::node_tag::fn_decl:
    return def_kind::fn;
  case ast::node_tag::extern_fn:
    return def_kind::extern_fn;
  case ast::node_tag::struct_decl:
    return def_kind::struct_;
  case ast::node_tag::static_decl:
    return def_kind::static_;
  case ast::node_tag::const_decl:
    return def_kind::const_;
  case ast::node_tag::type_alias:
    return def_kind::type_alias;
  case ast::node_tag::error_decl:
    return def_kind::error_name;

  default:
    return std::nullopt;
  }
}

inline ast::token_id decl_name_token(const ast::tree &tree, ast::node_id n) {
  return tree.main_token(n);
}

template <typename TokenText>
std::vector<decl_item> gather_decl_items(const ast::tree &tree,
                                         sema_context &cx,
                                         TokenText &&token_text) {
  std::vector<decl_item> out;
  for (ast::node_id n : tree.items()) {
    const auto kind = top_level_def_kind(tree.tag(n));
    if (!kind)
      continue;

    decl_item item;
    item.kind = *kind;
    item.is_public = (tree.flags(n) & ast::flag::is_public) != 0;
    item.name_token = decl_name_token(tree, n);
    item.name = item.name_token == ast::token_id::none
                    ? name_id::none
                    : cx.names.intern(token_text(item.name_token));
    item.decl = n;
    out.push_back(item);
  }

  return out;
}

} // namespace pit::sema
