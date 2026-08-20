#include "common/source.hpp"
#include "pit/lexer/lexer.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace pit::lexer;

namespace {

std::vector<token> lex_ok(std::string_view source) {
  lexer lex{source};
  auto result = lex.lex();
  EXPECT_TRUE(result.has_value())
      << "unexpected lex error: "
      << (result ? std::string{} : result.error().to_str());
  return result ? *result : std::vector<token>{};
}

std::vector<token_type> types_of(std::string_view source) {
  std::vector<token_type> types;
  for (const auto &tok : lex_ok(source))
    types.push_back(tok.type);
  return types;
}

} // namespace

TEST(Lexer, EmptySourceYieldsOnlyEof) {
  const auto types = types_of("");
  ASSERT_EQ(types.size(), 1u);
  EXPECT_EQ(types[0], token_type::eof);
}

TEST(Lexer, TwoCharacterOperatorsPreferLongestMatch) {
  EXPECT_EQ(types_of("+="),
            (std::vector{token_type::plus_equal, token_type::eof}));
  EXPECT_EQ(types_of("=="),
            (std::vector{token_type::double_equal, token_type::eof}));
  EXPECT_EQ(types_of("<="),
            (std::vector{token_type::less_equal, token_type::eof}));
}

TEST(Lexer, KeywordsRequireAWordBoundary) {
  const auto tokens = lex_ok("integer if");
  ASSERT_EQ(tokens.size(), 3u);
  EXPECT_EQ(tokens[0].type, token_type::identifier);
  EXPECT_EQ(tokens[0].text, "integer");
  EXPECT_EQ(tokens[1].type, token_type::kw_if);
}

TEST(Lexer, TracksLineAndColumn) {
  const auto tokens = lex_ok("a\n  b");
  ASSERT_GE(tokens.size(), 2u);
  EXPECT_EQ(tokens[0].line, 1u);
  EXPECT_EQ(tokens[0].column, 1u);
  EXPECT_EQ(tokens[1].line, 2u);
  EXPECT_EQ(tokens[1].column, 3u);
}

TEST(Lexer, ReportsUnterminatedString) {
  lexer lex{"\"oops"};
  const auto result = lex.lex();
  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error().kind, lex_error_kind::unterminated_string);
}

TEST(Lexer, LexesBenchmarkOne) {
  const auto source = cherry::common::source_file::load(
      std::string{CHERRY_FIXTURE_ROOT} + "/pit/benchmark_1.pit");
  ASSERT_TRUE(source.has_value()) << source.error();

  lexer lex{source->text()};
  const auto tokens = lex.lex();
  ASSERT_TRUE(tokens.has_value());

  ASSERT_FALSE(tokens->empty());
  EXPECT_EQ(tokens->front().type, token_type::kw_func);
  EXPECT_EQ(tokens->back().type, token_type::eof);

  // i32 must lex as an identifier, not a keyword.
  bool saw_i32 = false;
  for (const auto &tok : *tokens) {
    if (tok.text == "i32") {
      EXPECT_EQ(tok.type, token_type::identifier);
      saw_i32 = true;
    }
  }
  EXPECT_TRUE(saw_i32);
}
