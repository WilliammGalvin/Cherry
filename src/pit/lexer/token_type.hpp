#pragma once

#include <optional>
#include <string_view>
#include <utility>

namespace pit::lexer {

enum class token_type {
  // literals
  identifier,
  directive,
  integer_literal,
  float_literal,
  string_literal,
  boolean_literal_true,
  boolean_literal_false,

  // keywords
  kw_public,
  kw_private,
  kw_const,
  kw_if,
  kw_else,
  kw_while,
  kw_for,
  kw_func,
  kw_return,
  kw_continue,
  kw_break,

  // punctuation
  semi_colon,
  colon,
  comma,
  left_paren,
  right_paren,
  left_brace,
  right_brace,
  left_bracket,
  right_bracket,
  dot,

  // comparison
  greater,
  less,
  greater_equal,
  less_equal,

  // arithmetic
  plus,
  minus,
  star,
  slash,
  percent,

  // assignment
  equal,
  plus_equal,
  minus_equal,
  star_equal,
  slash_equal,
  percent_equal,

  // logical / equality
  double_equal,
  bang_equal,
  logical_or,
  logical_and,
  bang,

  // special
  invalid,
  eof,
};

constexpr std::string_view token_type_to_str(token_type type) noexcept {
  switch (type) {
  case token_type::identifier:
    return "identifier";
  case token_type::directive:
    return "directive";
  case token_type::integer_literal:
    return "integer_literal";
  case token_type::float_literal:
    return "float_literal";
  case token_type::string_literal:
    return "string_literal";
  case token_type::boolean_literal_true:
    return "boolean_literal_true";
  case token_type::boolean_literal_false:
    return "boolean_literal_false";
  case token_type::kw_public:
    return "kw_public";
  case token_type::kw_private:
    return "kw_private";
  case token_type::kw_const:
    return "kw_const";
  case token_type::kw_if:
    return "kw_if";
  case token_type::kw_else:
    return "kw_else";
  case token_type::kw_while:
    return "kw_while";
  case token_type::kw_for:
    return "kw_for";
  case token_type::kw_func:
    return "kw_func";
  case token_type::kw_return:
    return "kw_return";
  case token_type::kw_continue:
    return "kw_continue";
  case token_type::kw_break:
    return "kw_break";
  case token_type::semi_colon:
    return "semi_colon";
  case token_type::colon:
    return "colon";
  case token_type::comma:
    return "comma";
  case token_type::left_paren:
    return "left_paren";
  case token_type::right_paren:
    return "right_paren";
  case token_type::left_brace:
    return "left_brace";
  case token_type::right_brace:
    return "right_brace";
  case token_type::left_bracket:
    return "left_bracket";
  case token_type::right_bracket:
    return "right_bracket";
  case token_type::dot:
    return "dot";
  case token_type::greater:
    return "greater";
  case token_type::less:
    return "less";
  case token_type::greater_equal:
    return "greater_equal";
  case token_type::less_equal:
    return "less_equal";
  case token_type::plus:
    return "plus";
  case token_type::minus:
    return "minus";
  case token_type::star:
    return "star";
  case token_type::slash:
    return "slash";
  case token_type::percent:
    return "percent";
  case token_type::equal:
    return "equal";
  case token_type::plus_equal:
    return "plus_equal";
  case token_type::minus_equal:
    return "minus_equal";
  case token_type::star_equal:
    return "star_equal";
  case token_type::slash_equal:
    return "slash_equal";
  case token_type::percent_equal:
    return "percent_equal";
  case token_type::double_equal:
    return "double_equal";
  case token_type::bang_equal:
    return "bang_equal";
  case token_type::logical_or:
    return "logical_or";
  case token_type::logical_and:
    return "logical_and";
  case token_type::bang:
    return "bang";
  case token_type::invalid:
    return "invalid";
  case token_type::eof:
    return "eof";
  }

  std::unreachable();
}

constexpr std::optional<token_type>
keyword_from_text(std::string_view text) noexcept {
  if (text == "public")
    return token_type::kw_public;
  if (text == "private")
    return token_type::kw_private;
  if (text == "const")
    return token_type::kw_const;
  if (text == "if")
    return token_type::kw_if;
  if (text == "else")
    return token_type::kw_else;
  if (text == "while")
    return token_type::kw_while;
  if (text == "for")
    return token_type::kw_for;
  if (text == "fn")
    return token_type::kw_func;
  if (text == "return")
    return token_type::kw_return;
  if (text == "continue")
    return token_type::kw_continue;
  if (text == "break")
    return token_type::kw_break;
  if (text == "true")
    return token_type::boolean_literal_true;
  if (text == "false")
    return token_type::boolean_literal_false;

  return std::nullopt;
}

} // namespace pit::lexer
