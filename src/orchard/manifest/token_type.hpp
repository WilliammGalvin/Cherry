#pragma once

#include <common/lexing/token.hpp>

namespace orchard::manifest {

enum class token_type : common::lexing::base_token_type {
  identifier,

  integer_literal,
  float_literal,
  string_literal,
  boolean_literal_true,
  boolean_literal_false,

  // keywords
  kw_project,
  kw_version,
  kw_author,
  kw_authors,
  kw_source_dir,
  kw_entry,

  semi_colon,
  colon,
  comma,
  left_paren,
  right_paren,
  left_bracket,
  right_bracket,

  invalid,
  eof,
};

inline token_type keyword_to_token_type(std::string_view keyword) {
  static constexpr std::pair<std::string_view, token_type> keywords[] = {
      {"project", token_type::kw_project},
      {"version", token_type::kw_version},
      {"author", token_type::kw_author},
      {"authors", token_type::kw_authors},
      {"source_dir", token_type::kw_source_dir},
      {"entry", token_type::kw_entry},
  };

  for (const auto &[kw, type] : keywords) {
    if (keyword == kw)
      return type;
  }

  return token_type::invalid;
}

inline token_type symbol_to_token_type(char c) {
  static constexpr std::pair<char, token_type> symbols[] = {
      {';', token_type::semi_colon},    {':', token_type::colon},
      {',', token_type::comma},         {'(', token_type::left_paren},
      {')', token_type::right_paren},   {'[', token_type::left_bracket},
      {']', token_type::right_bracket},
  };

  for (const auto &[sym, type] : symbols) {
    if (c == sym)
      return type;
  }

  return token_type::invalid;
}

} // namespace orchard::manifest
