#pragma once

#include "context.hpp"
#include "def.hpp"
#include "pit/ast/node.hpp"
#include "scope.hpp"
#include "type.hpp"

#include <span>

namespace pit::sema {

struct decl_item {
  def_kind kind{def_kind::fn};
  bool is_public{false};
  name_id name{name_id::none};
  ast::token_id name_token{ast::token_id::none};
  ast::node_id decl{ast::node_id::none};
};

using module_scope = scope_stack<name_id, def_id>;

void collect_declarations(sema_context &cx, file_id file, def_id module,
                          std::span<const decl_item> items,
                          module_scope &scope);

} // namespace pit::sema
