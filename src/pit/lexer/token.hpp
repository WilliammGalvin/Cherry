#pragma once

#include "pit/lexer/token_type.hpp"

#include <cstdint>
#include <string>
#include <string_view>

namespace pit::lexer {

// A lexed token. `text` is a non-owning view into the source buffer, which
// must outlive every token produced from it. It is empty for tokens whose
// spelling is fully determined by the type (punctuation, keywords, eof) and
// non-empty for identifiers and literals.
struct token {
  token_type type = token_type::invalid;
  std::string_view text;
  std::uint32_t offset = 0; // byte offset from start of file
  std::uint32_t line = 1;   // 1-based
  std::uint32_t column = 1; // 1-based

  // Aggregate: brace- or paren-initialise as {type, text, offset, line,
  // column}.

  [[nodiscard]] constexpr std::string_view type_name() const noexcept {
    return token_type_to_str(type);
  }

  [[nodiscard]] std::string to_str() const;
};

bool operator==(const token &lhs, const token &rhs) noexcept;

// Lets GoogleTest print a readable token on failure instead of a byte dump.
void PrintTo(const token &tok, std::ostream *os);

} // namespace pit::lexer
