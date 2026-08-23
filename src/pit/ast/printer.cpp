#include "pit/ast/printer.hpp"

#include <sstream>

namespace pit::ast {

namespace {

class printer {
public:
  printer(const tree &t, std::span<const lexer::token> tokens)
      : tree_(t), tokens_(tokens) {}

  void walk(node_id id, int depth) {
    if (!present(id))
      return;

    const node &n = tree_[id];
    indent(depth);
    out_ << node_kind_to_str(n.kind);

    if (const auto text = token_text(n.main_token); !text.empty())
      out_ << " '" << text << '\'';
    out_ << '\n';

    switch (n.kind) {
    case node_kind::fn_decl:
    case node_kind::extern_fn:
      walk_fn(n, depth);
      break;
    case node_kind::block:
      walk_block(n, depth);
      break;
    case node_kind::call:
      walk_call(n, depth);
      break;
    case node_kind::if_else:
      walk_if_else(n, depth);
      break;
    default:
      walk(n.lhs, depth + 1);
      walk(n.rhs, depth + 1);
      break;
    }
  }

  std::string str() const { return out_.str(); }

private:
  void walk_fn(const node &n, int depth) {
    const auto extra = tree_.extra<fn_extra>(static_cast<extra_id>(n.lhs));
    for (node_id p : tree_.range(extra.params_start, extra.params_end))
      walk(p, depth + 1);

    indent(depth + 1);
    out_ << "-> " << token_text(tree_[extra.return_type].main_token) << '\n';

    walk(n.rhs, depth + 1);
  }

  void walk_block(const node &n, int depth) {
    const auto start = static_cast<std::uint32_t>(n.lhs);
    const auto end = static_cast<std::uint32_t>(n.rhs);
    for (node_id s : tree_.range(start, end))
      walk(s, depth + 1);
  }

  void walk_call(const node &n, int depth) {
    walk(n.lhs, depth + 1);
    const auto extra = tree_.extra<call_extra>(static_cast<extra_id>(n.rhs));
    for (node_id a : tree_.range(extra.args_start, extra.args_end))
      walk(a, depth + 1);
  }

  void walk_if_else(const node &n, int depth) {
    walk(n.lhs, depth + 1);
    const auto extra = tree_.extra<if_extra>(static_cast<extra_id>(n.rhs));
    walk(extra.then_block, depth + 1);
    walk(extra.else_block, depth + 1);
  }

  void indent(int depth) {
    for (int i = 0; i < depth; ++i)
      out_ << "  ";
  }

  std::string_view token_text(token_id id) const {
    const auto at = static_cast<std::uint32_t>(id);
    if (at >= tokens_.size())
      return {};

    return tokens_[at].text;
  }

  const tree &tree_;
  std::span<const lexer::token> tokens_;
  std::ostringstream out_;
};

} // namespace

std::string dump(const tree &t, std::span<const lexer::token> tokens) {
  printer p{t, tokens};
  for (node_id root : t.roots())
    p.walk(root, 0);

  return p.str();
}

} // namespace pit::ast
