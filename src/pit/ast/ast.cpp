#include "pit/ast/ast.hpp"

#include <utility>

namespace pit::ast {

std::string_view node_kind_to_str(node_kind kind) noexcept {
  switch (kind) {
  case node_kind::fn_decl:
    return "fn_decl";
  case node_kind::extern_fn:
    return "extern_fn";
  case node_kind::param:
    return "param";
  case node_kind::type_name:
    return "type_name";
  case node_kind::block:
    return "block";
  case node_kind::const_decl:
    return "const_decl";
  case node_kind::var_decl:
    return "var_decl";
  case node_kind::assign:
    return "assign";
  case node_kind::if_stmt:
    return "if_stmt";
  case node_kind::if_else:
    return "if_else";
  case node_kind::while_stmt:
    return "while_stmt";
  case node_kind::return_stmt:
    return "return_stmt";
  case node_kind::break_stmt:
    return "break_stmt";
  case node_kind::continue_stmt:
    return "continue_stmt";
  case node_kind::expr_stmt:
    return "expr_stmt";
  case node_kind::int_literal:
    return "int_literal";
  case node_kind::float_literal:
    return "float_literal";
  case node_kind::string_literal:
    return "string_literal";
  case node_kind::bool_literal:
    return "bool_literal";
  case node_kind::identifier:
    return "identifier";
  case node_kind::call:
    return "call";
  case node_kind::neg:
    return "neg";
  case node_kind::logical_not:
    return "logical_not";
  case node_kind::add:
    return "add";
  case node_kind::sub:
    return "sub";
  case node_kind::mul:
    return "mul";
  case node_kind::div:
    return "div";
  case node_kind::rem:
    return "rem";
  case node_kind::eq:
    return "eq";
  case node_kind::ne:
    return "ne";
  case node_kind::lt:
    return "lt";
  case node_kind::le:
    return "le";
  case node_kind::gt:
    return "gt";
  case node_kind::ge:
    return "ge";
  case node_kind::logical_and:
    return "logical_and";
  case node_kind::logical_or:
    return "logical_or";
  }
  std::unreachable();
}

bool is_binary(node_kind kind) noexcept {
  return kind >= node_kind::add && kind <= node_kind::logical_or;
}

} // namespace pit::ast
