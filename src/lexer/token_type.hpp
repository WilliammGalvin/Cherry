#pragma once

#include <string_view>

namespace cherry::lexer {

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
  kw_int,
  kw_float,
  kw_string,
  kw_bool,
  kw_if,
  kw_else,
  kw_while,
  kw_for,
  kw_func,
  kw_return,
  kw_void,
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
  line_end,
  eof,
};

constexpr std::string_view token_type_to_str(token_type type) noexcept;

} // namespace cherry::lexer
