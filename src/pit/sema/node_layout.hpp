#pragma once

#include "pit/ast/extra.hpp"
#include "pit/ast/tree.hpp"

namespace pit::sema::layout {

inline ast::node_id inner_type(const ast::tree &t, ast::node_id n) {
  return t.lhs(n);
}

inline ast::node_id array_elem(const ast::tree &t, ast::node_id n) {
  return t.lhs(n);
}

inline ast::node_id array_len(const ast::tree &t, ast::node_id n) {
  return t.rhs(n);
}

inline ast::node_id generic_base(const ast::tree &t, ast::node_id n) {
  return t.lhs(n);
}

inline ast::id_view<ast::node_id> generic_args(const ast::tree &t,
                                               ast::node_id n) {
  return t.range<ast::node_id>(t.data_range(t.rhs(n)));
}

inline ast::node_id alias_target(const ast::tree &t, ast::node_id n) {
  return t.lhs(n);
}

inline ast::node_id member_type(const ast::tree &t, ast::node_id n) {
  return t.lhs(n);
}

inline ast::fn_extra fn_ext(const ast::tree &t, ast::node_id n) {
  return t.extra<ast::fn_extra>(t.lhs_extra(n));
}

inline ast::struct_extra struct_ext(const ast::tree &t, ast::node_id n) {
  return t.extra<ast::struct_extra>(t.lhs_extra(n));
}

} // namespace pit::sema::layout
