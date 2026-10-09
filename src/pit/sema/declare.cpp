#include "declare.hpp"

namespace pit::sema {

void collect_declarations(sema_context &cx, file_id file, def_id module,
                          std::span<const decl_item> items,
                          module_scope &scope) {
  for (const decl_item &item : items) {
    if (!is_valid(item.name))
      continue;

    if (cx.builtin_type(item.name) != type_id::invalid) {
      cx.diags.error(file, item.name_token,
                     "'{}' is a builtin type and cannot be redefined",
                     cx.names.text(item.name));
      continue;
    }

    if (auto previous = scope.lookup_in_current(item.name)) {
      cx.diags.error(file, item.name_token, "duplicate definition of '{}'",
                     cx.names.text(item.name));
      cx.diags.note(file, cx.defs.get(*previous).name_token,
                    "first defined here");
      continue;
    }

    def d;
    d.kind = item.kind;
    d.is_public = item.is_public;
    d.name = item.name;
    d.name_token = item.name_token;
    d.file = file;
    d.decl = item.decl;
    d.parent = module;
    scope.declare(item.name, cx.defs.add(d));
  }
}

} // namespace pit::sema
