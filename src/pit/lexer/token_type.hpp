/// @file token_type.hpp
/// @brief Defines the `token_type` enum class, which represents the different
///        types of tokens that can be produced by the lexer.
#pragma once

#include <array>
#include <optional>
#include <string_view>
#include <utility>

namespace pit::lexer {

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

using keyword_entry =
    std::pair<std::string_view,
              token_type>; ///< Represents a mapping between a keyword string
                           ///< and its corresponding token_type.

/// @brief A constexpr array of keyword_entry pairs that maps recognized
///        keywords to their corresponding token_type values.
inline constexpr std::array keyword_table = {
    keyword_entry{"public", token_type::kw_public},
    keyword_entry{"private", token_type::kw_private},
    keyword_entry{"const", token_type::kw_const},
    keyword_entry{"if", token_type::kw_if},
    keyword_entry{"else", token_type::kw_else},
    keyword_entry{"while", token_type::kw_while},
    keyword_entry{"for", token_type::kw_for},
    keyword_entry{"fn", token_type::kw_func},
    keyword_entry{"return", token_type::kw_return},
    keyword_entry{"continue", token_type::kw_continue},
    keyword_entry{"break", token_type::kw_break},
    keyword_entry{"true", token_type::boolean_literal_true},
    keyword_entry{"false", token_type::boolean_literal_false}};

/// @brief Converts a string_view to its corresponding token_type if it is a
///        recognized keyword.
/// @param text The string_view to convert.
/// @return An optional containing the corresponding token_type if the
///         string_view is a recognized keyword, or std::nullopt if it is not.
constexpr std::optional<token_type>
keyword_from_text(std::string_view text) noexcept {
  for (const auto &[keyword, type] : keyword_table) {
    if (text == keyword)
      return type;
  }

  return std::nullopt;
}

/// @brief Converts a token_type to its string representation.
/// @param type The token_type to convert.
/// @return A string_view representing the string representation of the
///         token_type.
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

} // namespace pit::lexer
