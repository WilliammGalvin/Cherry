#pragma once

#include "context.hpp"
#include "declare.hpp"
#include "pit/ast/node.hpp"
#include "pit/ast/tree.hpp"

#include <functional>
#include <string_view>

namespace pit::sema {

using token_text_fn = std::function<std::string_view(ast::token_id)>;

void resolve_signatures(sema_context &cx, const ast::tree &tree, file_id file,
                        def_id module, module_scope &scope,
                        const token_text_fn &token_text);

} // namespace pit::sema
