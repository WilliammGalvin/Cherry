#pragma once

#include <algorithm>
#include <common/io/source_manager.hpp>
#include <orchard/manifest/lexer.hpp>
#include <orchard/manifest/parser.hpp>
#include <string_view>

namespace orchard::manifest {

inline manifest_data from_source(common::io::source_id id,
                                 const common::io::source_manager &sm) {
  const auto source = sm.get_source(id);
  if (source.text.empty())
    return {};

  const auto lex_res = lexer(source).lex();
  if (!lex_res)
    throw std::runtime_error("Failed to lex manifest source");

  const auto parse_res = parser(lex_res.value()).parse();
  if (!parse_res)
    throw std::runtime_error("Failed to parse manifest source");

  return parse_res.value();
}

inline static bool has_entry(const manifest_data &data,
                             const std::string_view key) {
  return std::ranges::any_of(
      data, [&](const manifest_entry &entry) { return entry.first == key; });
}

} // namespace orchard::manifest
