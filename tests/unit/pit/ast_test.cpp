#include "pit/ast/ast.hpp"
#include "pit/ast/printer.hpp"
#include "pit/lexer/lexer.hpp"

#include <gtest/gtest.h>

#include <set>
#include <string>
#include <vector>

using namespace pit;
using namespace pit::ast;

namespace {

class lexed {
public:
  explicit lexed(std::string source) : source_(std::move(source)) {
    lexer::lexer lex{source_};
    auto result = lex.lex();
    EXPECT_TRUE(result.has_value());

    if (result)
      tokens_ = std::move(*result);
  }

  std::span<const lexer::token> tokens() const { return tokens_; }

  token_id at(std::string_view text, int nth = 0) const {
    int seen = 0;
    for (std::uint32_t i = 0; i < tokens_.size(); ++i)
      if (tokens_[i].text == text && seen++ == nth)
        return static_cast<token_id>(i);

    ADD_FAILURE() << "no token with text '" << text << "' at index " << nth;
    return token_id{0};
  }

private:
  std::string source_;
  std::vector<lexer::token> tokens_;
};

constexpr auto first_kind = node_kind::fn_decl;
constexpr auto last_kind = node_kind::logical_or;

std::vector<node_kind> all_kinds() {
  std::vector<node_kind> kinds;
  for (auto i = static_cast<std::uint8_t>(first_kind);
       i <= static_cast<std::uint8_t>(last_kind); ++i)
    kinds.push_back(static_cast<node_kind>(i));

  return kinds;
}

} // namespace

// --- tree invariants --------------------------------------------------------

TEST(AstTree, NodeZeroIsReservedAsAbsent) {
  const tree t;
  EXPECT_EQ(t.node_count(), 1u);
  EXPECT_FALSE(present(node_id::none));
}

TEST(AstTree, AddReturnsDistinctNonZeroIds) {
  tree t;
  const auto a = t.add(node_kind::identifier, token_id{0});
  const auto b = t.add(node_kind::identifier, token_id{1});

  EXPECT_TRUE(present(a));
  EXPECT_TRUE(present(b));
  EXPECT_NE(a, b);
  EXPECT_EQ(t.node_count(), 3u);
}

TEST(AstTree, StoresAndReadsBackNodeFields) {
  tree t;
  const auto lhs = t.add(node_kind::identifier, token_id{7});
  const auto rhs = t.add(node_kind::int_literal, token_id{9});
  const auto sum = t.add(node_kind::add, token_id{8}, lhs, rhs);

  const node &n = t[sum];
  EXPECT_EQ(n.kind, node_kind::add);
  EXPECT_EQ(n.main_token, token_id{8});
  EXPECT_EQ(n.lhs, lhs);
  EXPECT_EQ(n.rhs, rhs);
}

TEST(AstTree, AbsentChildrenDefaultToNone) {
  tree t;
  const auto ret = t.add(node_kind::return_stmt, token_id{0});
  EXPECT_FALSE(present(t[ret].lhs));
  EXPECT_FALSE(present(t[ret].rhs));
}

TEST(AstTree, RootsPreserveSourceOrder) {
  tree t;
  const auto a = t.add(node_kind::fn_decl, token_id{0});
  const auto b = t.add(node_kind::fn_decl, token_id{1});
  t.add_root(a);
  t.add_root(b);

  ASSERT_EQ(t.roots().size(), 2u);
  EXPECT_EQ(t.roots()[0], a);
  EXPECT_EQ(t.roots()[1], b);
}

// --- extra_data -------------------------------------------------------------

TEST(AstExtra, RangeRoundTrips) {
  tree t;
  const node_id ids[] = {t.add(node_kind::identifier, token_id{1}),
                         t.add(node_kind::identifier, token_id{2}),
                         t.add(node_kind::identifier, token_id{3})};

  const auto [start, end] = t.add_range(ids);
  const auto read = t.range(start, end);

  ASSERT_EQ(read.size(), 3u);
  EXPECT_EQ(read[0], ids[0]);
  EXPECT_EQ(read[1], ids[1]);
  EXPECT_EQ(read[2], ids[2]);
}

TEST(AstExtra, EmptyRangeIsValidAndEmpty) {
  tree t;
  const auto [start, end] = t.add_range({});
  EXPECT_EQ(start, end);
  EXPECT_TRUE(t.range(start, end).empty());
}

TEST(AstExtra, IndependentRangesDoNotOverlap) {
  tree t;
  const node_id first[] = {t.add(node_kind::identifier, token_id{1})};
  const node_id second[] = {t.add(node_kind::identifier, token_id{2}),
                            t.add(node_kind::identifier, token_id{3})};

  const auto [s1, e1] = t.add_range(first);
  const auto [s2, e2] = t.add_range(second);

  EXPECT_EQ(t.range(s1, e1).size(), 1u);
  EXPECT_EQ(t.range(s2, e2).size(), 2u);
  EXPECT_EQ(t.range(s1, e1)[0], first[0]);
  EXPECT_EQ(t.range(s2, e2)[0], second[0]);
}

TEST(AstExtra, FnExtraRoundTrips) {
  tree t;
  const auto ret_ty = t.add(node_kind::type_name, token_id{4});
  const auto id = t.add_extra(fn_extra{2, 5, ret_ty});

  const auto read = t.extra<fn_extra>(id);
  EXPECT_EQ(read.params_start, 2u);
  EXPECT_EQ(read.params_end, 5u);
  EXPECT_EQ(read.return_type, ret_ty);
}

TEST(AstExtra, IfExtraRoundTrips) {
  tree t;
  const auto then_block = t.add(node_kind::block, token_id{0});
  const auto else_block = t.add(node_kind::block, token_id{1});
  const auto id = t.add_extra(if_extra{then_block, else_block});

  const auto read = t.extra<if_extra>(id);
  EXPECT_EQ(read.then_block, then_block);
  EXPECT_EQ(read.else_block, else_block);
}

TEST(AstExtra, CallExtraRoundTripsThroughNodeRhs) {
  tree t;
  const auto callee = t.add(node_kind::identifier, token_id{0});
  const auto id = t.add_extra(call_extra{3, 7});
  const auto call_node =
      t.add(node_kind::call, token_id{1}, callee, static_cast<node_id>(id));

  const auto read = t.extra_of<call_extra>(call_node);
  EXPECT_EQ(read.args_start, 3u);
  EXPECT_EQ(read.args_end, 7u);
}

// --- node_kind table --------------------------------------------------------

TEST(AstNodeKind, EveryKindHasAUniqueNonEmptyName) {
  std::set<std::string_view> seen;
  for (const node_kind kind : all_kinds()) {
    const auto name = node_kind_to_str(kind);
    EXPECT_FALSE(name.empty())
        << "kind " << static_cast<int>(kind) << " has an empty name";
    EXPECT_TRUE(seen.insert(name).second) << "duplicate name: " << name;
  }
}

TEST(AstNodeKind, IsBinaryCoversExactlyTheBinaryOperators) {
  const std::set<node_kind> expected = {
      node_kind::add,       node_kind::sub, node_kind::mul,
      node_kind::div,       node_kind::rem, node_kind::eq,
      node_kind::ne,        node_kind::lt,  node_kind::le,
      node_kind::gt,        node_kind::ge,  node_kind::logical_and,
      node_kind::logical_or};

  for (const node_kind kind : all_kinds())
    EXPECT_EQ(is_binary(kind), expected.contains(kind))
        << "mismatch for " << node_kind_to_str(kind);
}

TEST(AstNode, IsSixteenBytesAndTriviallyDestructible) {
  EXPECT_EQ(sizeof(node), 16u);
  EXPECT_TRUE(std::is_trivially_destructible_v<node>);
  EXPECT_TRUE(std::is_trivially_copyable_v<node>);
}

// --- printer ----------------------------------------------------------------

TEST(AstPrinter, EmptyTreePrintsNothing) {
  const tree t;
  const lexed src{""};
  EXPECT_EQ(dump(t, src.tokens()), "");
}

TEST(AstPrinter, PrintsBinaryExpressionWithOperands) {
  const lexed src{"a + b"};
  tree t;
  const auto lhs = t.add(node_kind::identifier, src.at("a"));
  const auto rhs = t.add(node_kind::identifier, src.at("b"));
  t.add_root(t.add(node_kind::add, src.at("+"), lhs, rhs));

  EXPECT_EQ(dump(t, src.tokens()), "add '+'\n"
                                   "  identifier 'a'\n"
                                   "  identifier 'b'\n");
}

TEST(AstPrinter, PrintsReturnWithNoValue) {
  const lexed src{"return;"};
  tree t;
  t.add_root(t.add(node_kind::return_stmt, src.at("return")));

  EXPECT_EQ(dump(t, src.tokens()), "return_stmt 'return'\n");
}

TEST(AstPrinter, PrintsBlockStatementsInOrder) {
  const lexed src{"{ break; continue; }"};
  tree t;
  const node_id stmts[] = {t.add(node_kind::break_stmt, src.at("break")),
                           t.add(node_kind::continue_stmt, src.at("continue"))};
  const auto [start, end] = t.add_range(stmts);
  t.add_root(t.add(node_kind::block, src.at("{"), static_cast<node_id>(start),
                   static_cast<node_id>(end)));

  EXPECT_EQ(dump(t, src.tokens()), "block '{'\n"
                                   "  break_stmt 'break'\n"
                                   "  continue_stmt 'continue'\n");
}

TEST(AstPrinter, PrintsCallWithArguments) {
  const lexed src{"add(2, 5)"};
  tree t;
  const auto callee = t.add(node_kind::identifier, src.at("add"));
  const node_id args[] = {t.add(node_kind::int_literal, src.at("2")),
                          t.add(node_kind::int_literal, src.at("5"))};
  const auto [start, end] = t.add_range(args);
  const auto extra = t.add_extra(call_extra{start, end});
  t.add_root(
      t.add(node_kind::call, src.at("("), callee, static_cast<node_id>(extra)));

  EXPECT_EQ(dump(t, src.tokens()), "call '('\n"
                                   "  identifier 'add'\n"
                                   "  int_literal '2'\n"
                                   "  int_literal '5'\n");
}

TEST(AstPrinter, PrintsFunctionDeclarationWithParamsAndReturnType) {
  const lexed src{"fn add(i32 a, i32 b): i32 { return a + b; }"};
  tree t;

  const auto ty_a = t.add(node_kind::type_name, src.at("i32", 0));
  const auto p_a = t.add(node_kind::param, src.at("a", 0), ty_a);
  const auto ty_b = t.add(node_kind::type_name, src.at("i32", 1));
  const auto p_b = t.add(node_kind::param, src.at("b", 0), ty_b);
  const auto ret_ty = t.add(node_kind::type_name, src.at("i32", 2));

  const node_id params[] = {p_a, p_b};
  const auto [ps, pe] = t.add_range(params);

  const auto sum = t.add(node_kind::add, src.at("+"),
                         t.add(node_kind::identifier, src.at("a", 1)),
                         t.add(node_kind::identifier, src.at("b", 1)));
  const auto ret = t.add(node_kind::return_stmt, src.at("return"), sum);

  const node_id stmts[] = {ret};
  const auto [bs, be] = t.add_range(stmts);
  const auto body = t.add(node_kind::block, src.at("{"),
                          static_cast<node_id>(bs), static_cast<node_id>(be));

  const auto fx = t.add_extra(fn_extra{ps, pe, ret_ty});
  t.add_root(
      t.add(node_kind::fn_decl, src.at("add"), static_cast<node_id>(fx), body));

  EXPECT_EQ(dump(t, src.tokens()), "fn_decl 'add'\n"
                                   "  param 'a'\n"
                                   "    type_name 'i32'\n"
                                   "  param 'b'\n"
                                   "    type_name 'i32'\n"
                                   "  -> i32\n"
                                   "  block '{'\n"
                                   "    return_stmt 'return'\n"
                                   "      add '+'\n"
                                   "        identifier 'a'\n"
                                   "        identifier 'b'\n");
}

TEST(AstPrinter, PrintsIfElseBranches) {
  const lexed src{"if x { break; } else { continue; }"};
  tree t;

  const node_id then_stmts[] = {t.add(node_kind::break_stmt, src.at("break"))};
  const auto [ts, te] = t.add_range(then_stmts);
  const auto then_block =
      t.add(node_kind::block, src.at("{", 0), static_cast<node_id>(ts),
            static_cast<node_id>(te));

  const node_id else_stmts[] = {
      t.add(node_kind::continue_stmt, src.at("continue"))};
  const auto [es, ee] = t.add_range(else_stmts);
  const auto else_block =
      t.add(node_kind::block, src.at("{", 1), static_cast<node_id>(es),
            static_cast<node_id>(ee));

  const auto cond = t.add(node_kind::identifier, src.at("x"));
  const auto extra = t.add_extra(if_extra{then_block, else_block});
  t.add_root(t.add(node_kind::if_else, src.at("if"), cond,
                   static_cast<node_id>(extra)));

  EXPECT_EQ(dump(t, src.tokens()), "if_else 'if'\n"
                                   "  identifier 'x'\n"
                                   "  block '{'\n"
                                   "    break_stmt 'break'\n"
                                   "  block '{'\n"
                                   "    continue_stmt 'continue'\n");
}

TEST(AstPrinter, PrintsAllRootsInOrder) {
  const lexed src{"a b"};
  tree t;
  t.add_root(t.add(node_kind::identifier, src.at("a")));
  t.add_root(t.add(node_kind::identifier, src.at("b")));

  EXPECT_EQ(dump(t, src.tokens()), "identifier 'a'\n"
                                   "identifier 'b'\n");
}
