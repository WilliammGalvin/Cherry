#pragma once

#include "pit/lexer/token.hpp"

#include <cstdint>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

namespace pit::ast {

enum class node_id : std::uint32_t { none = 0 };
enum class token_id : std::uint32_t {};
enum class extra_id : std::uint32_t {};

constexpr bool present(node_id id) noexcept { return id != node_id::none; }

enum class node_kind : std::uint8_t {
  // -- declarations -------------------------------------------------------
  fn_decl,   // main: name  lhs: extra->fn_extra   rhs: body block
  extern_fn, // main: name  lhs: extra->fn_extra   rhs: none
  param,     // main: name  lhs: type              rhs: none

  // --- types --------------------------------------------------------------
  type_name, // main: identifier token ("i32", "bool", ...)

  // --- statements ---------------------------------------------------------
  block,       // lhs: extra range start  rhs: extra range end
  const_decl,  // main: name  lhs: type   rhs: init (may be none)
  var_decl,    // main: name  lhs: type   rhs: init (may be none)
  assign,      // main: '='   lhs: target rhs: value
  if_stmt,     // main: 'if'  lhs: cond   rhs: then block
  if_else,     // main: 'if'  lhs: cond   rhs: extra->if_extra
  while_stmt,  // main: 'while' lhs: cond rhs: body block
  return_stmt, // main: 'return' lhs: value (may be none)
  break_stmt,  // main: 'break'
  continue_stmt,
  expr_stmt, // lhs: expr

  // --- expressions --------------------------------------------------------
  int_literal, // main: the literal token; value parsed on demand
  float_literal,
  string_literal,
  bool_literal, // main: 'true' or 'false'
  identifier,   // main: the name token

  call, // main: '('  lhs: callee  rhs: extra->call_extra

  // Unary: lhs is the operand.
  neg,
  logical_not,

  // Binary: lhs and rhs are the operands, main is the operator token.
  // The operator lives in the tag, so there is no separate op field and
  // every switch over binary nodes is exhaustively checked.
  add,
  sub,
  mul,
  div,
  rem,
  eq,
  ne,
  lt,
  le,
  gt,
  ge,
  logical_and,
  logical_or,
};

std::string_view node_kind_to_str(node_kind kind) noexcept;
bool is_binary(node_kind kind) noexcept;

struct node {
  node_kind kind{};
  token_id main_token{};
  node_id lhs{};
  node_id rhs{};
};

static_assert(sizeof(node) == 16);
static_assert(std::is_trivially_destructible_v<node>);

struct fn_extra {
  std::uint32_t params_start;
  std::uint32_t params_end;
  node_id return_type;
};

struct if_extra {
  node_id then_block;
  node_id else_block;
};

struct call_extra {
  std::uint32_t args_start;
  std::uint32_t args_end;
};

class tree {
public:
  tree() { nodes_.push_back(node{}); }

  const node &operator[](node_id id) const noexcept {
    return nodes_[static_cast<std::uint32_t>(id)];
  }

  node &operator[](node_id id) noexcept {
    return nodes_[static_cast<std::uint32_t>(id)];
  }

  std::uint32_t node_count() const noexcept {
    return static_cast<std::uint32_t>(nodes_.size());
  }

  std::span<const node_id> roots() const noexcept { return roots_; }

  void add_root(node_id id) { roots_.push_back(id); }

  node_id add(node_kind kind, token_id main, node_id lhs = node_id::none,
              node_id rhs = node_id::none) {
    nodes_.push_back(node{kind, main, lhs, rhs});
    return static_cast<node_id>(nodes_.size() - 1);
  }

  std::uint32_t extra_size() const noexcept {
    return static_cast<std::uint32_t>(extra_.size());
  }

  std::pair<std::uint32_t, std::uint32_t>
  add_range(std::span<const node_id> ids) {
    const auto start = extra_size();
    for (node_id id : ids)
      extra_.push_back(static_cast<std::uint32_t>(id));

    return {start, extra_size()};
  }

  std::span<const node_id> range(std::uint32_t start, std::uint32_t end) const {
    return {reinterpret_cast<const node_id *>(extra_.data()) + start,
            end - start};
  }

  template <typename T> extra_id add_extra(const T &value) {
    static_assert(sizeof(T) % sizeof(std::uint32_t) == 0);
    const auto at = extra_size();
    const auto *words = reinterpret_cast<const std::uint32_t *>(&value);
    for (std::size_t i = 0; i < sizeof(T) / sizeof(std::uint32_t); ++i)
      extra_.push_back(words[i]);

    return static_cast<extra_id>(at);
  }

  template <typename T> [[nodiscard]] T extra(extra_id at) const {
    T value;
    auto *words = reinterpret_cast<std::uint32_t *>(&value);
    const auto offset = static_cast<std::uint32_t>(at);
    for (std::size_t i = 0; i < sizeof(T) / sizeof(std::uint32_t); ++i)
      words[i] = extra_[offset + i];

    return value;
  }

  template <typename T> [[nodiscard]] T extra_of(node_id id) const {
    return extra<T>(static_cast<extra_id>((*this)[id].rhs));
  }

private:
  std::vector<node> nodes_;
  std::vector<std::uint32_t> extra_;
  std::vector<node_id> roots_;
};

} // namespace pit::ast
