#include "pit/ast/printer.hpp"
#include "pit/lexer/lexer.hpp"
#include "pit/parser/parser.hpp"

#include <gtest/gtest.h>

#include <sstream>

#include <string>
#include <vector>

using namespace pit;

namespace {

// Owns the source buffer and tokens for the lifetime of a parse, since both
// tokens and AST nodes reference them.
class parsed {
public:
  explicit parsed(std::string source) : source_(std::move(source)) {
    lexer::lexer lex{source_};
    auto tokens = lex.lex();
    if (!tokens) {
      lex_error_ = tokens.error().to_str();
      return;
    }
    tokens_ = std::move(*tokens);

    parser::parser p{tokens_};
    auto tree = p.parse();
    if (!tree) {
      parse_error_ = tree.error();
      return;
    }
    tree_ = std::move(*tree);
    ok_ = true;
  }

  [[nodiscard]] bool ok() const { return ok_; }
  [[nodiscard]] const parser::parse_error &error() const {
    return parse_error_;
  }
  [[nodiscard]] const std::string &lex_error() const { return lex_error_; }

  [[nodiscard]] std::string dump() const { return ast::dump(tree_, tokens_); }

  [[nodiscard]] const ast::tree &tree() const { return tree_; }

private:
  std::string source_;
  std::vector<lexer::token> tokens_;
  ast::tree tree_;
  parser::parse_error parse_error_;
  std::string lex_error_;
  bool ok_ = false;
};

// Parses an expression by wrapping it in a minimal function, then returns just
// the dumped expression subtree so tests stay readable.
std::string expr_dump(const std::string &expression) {
  const parsed p{"fn f(): i32 { return " + expression + "; }"};
  if (!p.ok()) {
    ADD_FAILURE() << "parse failed: " << p.error().to_str() << p.lex_error();
    return {};
  }

  // Strip the function/block/return scaffolding and re-indent to column 0.
  std::string out;
  std::istringstream in{p.dump()};
  for (std::string line; std::getline(in, line);) {
    if (line.find("return_stmt") != std::string::npos) {
      while (std::getline(in, line)) {
        out += line.substr(6);
        out += '\n';
      }
      break;
    }
  }
  return out;
}

std::string parse_failure(const std::string &source) {
  const parsed p{source};
  EXPECT_FALSE(p.ok()) << "expected a parse error but parsing succeeded";
  return p.ok() ? std::string{} : p.error().message;
}

} // namespace

// --- declarations -----------------------------------------------------------

TEST(Parser, ParsesBenchmarkOne) {
  const parsed p{"fn add(i32 a, i32 b): i32 {\n"
                 "  return a + b;\n"
                 "}\n"
                 "fn main(): i32 {\n"
                 "  const i32 res = add(2, 5);\n"
                 "  return (res - 7);\n"
                 "}\n"};

  ASSERT_TRUE(p.ok()) << p.error().to_str() << p.lex_error();
  EXPECT_EQ(p.dump(), "fn_decl 'add'\n"
                      "  param 'a'\n"
                      "    type_name 'i32'\n"
                      "  param 'b'\n"
                      "    type_name 'i32'\n"
                      "  -> i32\n"
                      "  block '{'\n"
                      "    return_stmt 'return'\n"
                      "      add '+'\n"
                      "        identifier 'a'\n"
                      "        identifier 'b'\n"
                      "fn_decl 'main'\n"
                      "  -> i32\n"
                      "  block '{'\n"
                      "    const_decl 'res'\n"
                      "      type_name 'i32'\n"
                      "      call '('\n"
                      "        identifier 'add'\n"
                      "        int_literal '2'\n"
                      "        int_literal '5'\n"
                      "    return_stmt 'return'\n"
                      "      sub '-'\n"
                      "        identifier 'res'\n"
                      "        int_literal '7'\n");
}

TEST(Parser, ParsesFunctionWithNoParameters) {
  const parsed p{"fn f(): i32 { return 0; }"};
  ASSERT_TRUE(p.ok()) << p.error().to_str();
  EXPECT_NE(p.dump().find("fn_decl 'f'"), std::string::npos);
  EXPECT_EQ(p.dump().find("param"), std::string::npos);
}

TEST(Parser, ParsesMultipleTopLevelFunctionsInOrder) {
  const parsed p{"fn a(): i32 { return 0; } fn b(): i32 { return 1; }"};
  ASSERT_TRUE(p.ok()) << p.error().to_str();
  const auto dump = p.dump();
  EXPECT_LT(dump.find("fn_decl 'a'"), dump.find("fn_decl 'b'"));
}

// --- expression precedence and associativity --------------------------------

TEST(Parser, MultiplicationBindsTighterThanAddition) {
  EXPECT_EQ(expr_dump("1 + 2 * 3"), "add '+'\n"
                                    "  int_literal '1'\n"
                                    "  mul '*'\n"
                                    "    int_literal '2'\n"
                                    "    int_literal '3'\n");
}

TEST(Parser, ParenthesesOverridePrecedenceWithoutAddingANode) {
  EXPECT_EQ(expr_dump("(1 + 2) * 3"), "mul '*'\n"
                                      "  add '+'\n"
                                      "    int_literal '1'\n"
                                      "    int_literal '2'\n"
                                      "  int_literal '3'\n");
}

TEST(Parser, SubtractionIsLeftAssociative) {
  // Must group as (1 - 2) - 3, not 1 - (2 - 3).
  EXPECT_EQ(expr_dump("1 - 2 - 3"), "sub '-'\n"
                                    "  sub '-'\n"
                                    "    int_literal '1'\n"
                                    "    int_literal '2'\n"
                                    "  int_literal '3'\n");
}

TEST(Parser, ComparisonBindsTighterThanEquality) {
  EXPECT_EQ(expr_dump("a < b == c"), "eq '=='\n"
                                     "  lt '<'\n"
                                     "    identifier 'a'\n"
                                     "    identifier 'b'\n"
                                     "  identifier 'c'\n");
}

TEST(Parser, LogicalAndBindsTighterThanLogicalOr) {
  EXPECT_EQ(expr_dump("a || b && c"), "logical_or '||'\n"
                                      "  identifier 'a'\n"
                                      "  logical_and '&&'\n"
                                      "    identifier 'b'\n"
                                      "    identifier 'c'\n");
}

TEST(Parser, EqualityBindsTighterThanLogicalAnd) {
  EXPECT_EQ(expr_dump("a == b && c"), "logical_and '&&'\n"
                                      "  eq '=='\n"
                                      "    identifier 'a'\n"
                                      "    identifier 'b'\n"
                                      "  identifier 'c'\n");
}

TEST(Parser, UnaryBindsTighterThanBinary) {
  EXPECT_EQ(expr_dump("-a + b"), "add '+'\n"
                                 "  neg '-'\n"
                                 "    identifier 'a'\n"
                                 "  identifier 'b'\n");
}

TEST(Parser, UnaryOperatorsNest) {
  EXPECT_EQ(expr_dump("!!a"), "logical_not '!'\n"
                              "  logical_not '!'\n"
                              "    identifier 'a'\n");
}

// --- calls ------------------------------------------------------------------

TEST(Parser, ParsesCallWithNoArguments) {
  EXPECT_EQ(expr_dump("f()"), "call '('\n"
                              "  identifier 'f'\n");
}

TEST(Parser, ParsesNestedCallsAsArguments) {
  EXPECT_EQ(expr_dump("f(g(1), 2)"), "call '('\n"
                                     "  identifier 'f'\n"
                                     "  call '('\n"
                                     "    identifier 'g'\n"
                                     "    int_literal '1'\n"
                                     "  int_literal '2'\n");
}

TEST(Parser, ParsesChainedCalls) {
  EXPECT_EQ(expr_dump("f()()"), "call '('\n"
                                "  call '('\n"
                                "    identifier 'f'\n");
}

// --- statements -------------------------------------------------------------

TEST(Parser, DistinguishesDeclarationFromAssignment) {
  const parsed p{"fn f(): i32 { i32 x = 1; x = 2; return x; }"};
  ASSERT_TRUE(p.ok()) << p.error().to_str();
  const auto dump = p.dump();
  EXPECT_NE(dump.find("var_decl 'x'"), std::string::npos);
  EXPECT_NE(dump.find("assign '='"), std::string::npos);
}

TEST(Parser, ConstAndNonConstDeclarationsUseDifferentKinds) {
  const parsed constant{"fn f(): i32 { const i32 x = 1; return x; }"};
  const parsed variable{"fn f(): i32 { i32 x = 1; return x; }"};
  ASSERT_TRUE(constant.ok());
  ASSERT_TRUE(variable.ok());

  EXPECT_NE(constant.dump().find("const_decl 'x'"), std::string::npos);
  EXPECT_NE(variable.dump().find("var_decl 'x'"), std::string::npos);
}

TEST(Parser, DeclarationWithoutInitializerHasNoValueChild) {
  const parsed p{"fn f(): i32 { i32 x; return x; }"};
  ASSERT_TRUE(p.ok()) << p.error().to_str();
  EXPECT_NE(
      p.dump().find("var_decl 'x'\n      type_name 'i32'\n    return_stmt"),
      std::string::npos)
      << p.dump();
}

TEST(Parser, CompoundAssignmentDesugarsToBinaryOperation) {
  const parsed p{"fn f(): i32 { i32 x = 1; x += 2; return x; }"};
  ASSERT_TRUE(p.ok()) << p.error().to_str();
  EXPECT_NE(p.dump().find("assign '+='\n"
                          "      identifier 'x'\n"
                          "      add '+='\n"
                          "        identifier 'x'\n"
                          "        int_literal '2'\n"),
            std::string::npos)
      << p.dump();
}

TEST(Parser, ReturnWithoutValueHasNoChild) {
  const parsed p{"fn f(): i32 { return; }"};
  ASSERT_TRUE(p.ok()) << p.error().to_str();
  EXPECT_EQ(p.dump(), "fn_decl 'f'\n"
                      "  -> i32\n"
                      "  block '{'\n"
                      "    return_stmt 'return'\n");
}

TEST(Parser, ParsesIfWithoutElse) {
  const parsed p{"fn f(): i32 { if a { return 1; } return 0; }"};
  ASSERT_TRUE(p.ok()) << p.error().to_str();
  EXPECT_NE(p.dump().find("if_stmt 'if'"), std::string::npos);
  EXPECT_EQ(p.dump().find("if_else"), std::string::npos);
}

TEST(Parser, ParsesIfElse) {
  const parsed p{"fn f(): i32 { if a { return 1; } else { return 0; } }"};
  ASSERT_TRUE(p.ok()) << p.error().to_str();
  EXPECT_NE(p.dump().find("if_else 'if'"), std::string::npos);
}

TEST(Parser, ElseIfNestsInTheElseSlot) {
  const parsed p{"fn f(): i32 { if a { return 1; } else if b { return 2; } "
                 "else { return 3; } }"};
  ASSERT_TRUE(p.ok()) << p.error().to_str();

  const auto dump = p.dump();
  const auto outer = dump.find("if_else 'if'");
  const auto inner = dump.find("if_else 'if'", outer + 1);
  ASSERT_NE(inner, std::string::npos) << dump;
  // The nested if is indented deeper than the outer one.
  EXPECT_GT(dump.rfind('\n', inner) + 1, outer);
}

TEST(Parser, ParsesWhileLoopWithBreakAndContinue) {
  const parsed p{
      "fn f(): i32 { while a < 10 { if a { break; } continue; } return 0; }"};
  ASSERT_TRUE(p.ok()) << p.error().to_str();

  const auto dump = p.dump();
  EXPECT_NE(dump.find("while_stmt 'while'"), std::string::npos);
  EXPECT_NE(dump.find("break_stmt 'break'"), std::string::npos);
  EXPECT_NE(dump.find("continue_stmt 'continue'"), std::string::npos);
}

TEST(Parser, ParsesNestedBlocks) {
  const parsed p{"fn f(): i32 { { { return 0; } } }"};
  ASSERT_TRUE(p.ok()) << p.error().to_str();
  EXPECT_EQ(p.dump(), "fn_decl 'f'\n"
                      "  -> i32\n"
                      "  block '{'\n"
                      "    block '{'\n"
                      "      block '{'\n"
                      "        return_stmt 'return'\n"
                      "          int_literal '0'\n");
}

TEST(Parser, ParsesCallAsAStatement) {
  const parsed p{"fn f(): i32 { g(1); return 0; }"};
  ASSERT_TRUE(p.ok()) << p.error().to_str();
  EXPECT_NE(p.dump().find("expr_stmt"), std::string::npos);
}

// --- errors -----------------------------------------------------------------

TEST(Parser, RejectsMissingSemicolon) {
  EXPECT_NE(parse_failure("fn f(): i32 { return 0 }").find("';'"),
            std::string::npos);
}

TEST(Parser, RejectsUnclosedParenthesis) {
  EXPECT_NE(parse_failure("fn f(): i32 { return (1; }").find("')'"),
            std::string::npos);
}

TEST(Parser, RejectsUnclosedBlock) {
  EXPECT_FALSE(parse_failure("fn f(): i32 { return 0;").empty());
}

TEST(Parser, RejectsMissingReturnType) {
  EXPECT_NE(parse_failure("fn f() { return 0; }").find("':'"),
            std::string::npos);
}

TEST(Parser, RejectsAssignmentToNonIdentifier) {
  EXPECT_NE(
      parse_failure("fn f(): i32 { 1 = 2; return 0; }").find("assignable"),
      std::string::npos);
}

TEST(Parser, RejectsTopLevelStatement) {
  EXPECT_NE(parse_failure("return 0;").find("top-level"), std::string::npos);
}

TEST(Parser, RejectsMissingExpression) {
  EXPECT_NE(parse_failure("fn f(): i32 { return +; }").find("expression"),
            std::string::npos);
}

TEST(Parser, ErrorCarriesSourcePosition) {
  const parsed p{"fn f(): i32 {\n  return 0\n}"};
  ASSERT_FALSE(p.ok());
  EXPECT_EQ(p.error().line, 3u);
}
