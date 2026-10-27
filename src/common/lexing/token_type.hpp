/// @file token_type.hpp
/// @brief Defines the `token_type` enum class, which represents the different
///        types of tokens that can be produced by the lexer.
#pragma once

namespace common::lexing {

/// @brief Represents the different types of tokens that can be produced by the
///        lexer.
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

} // namespace common::lexing
