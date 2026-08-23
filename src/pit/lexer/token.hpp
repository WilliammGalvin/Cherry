#pragma once

#include "pit/lexer/token_type.hpp"

#include <cstdint>
#include <string>
#include <string_view>

namespace pit::lexer {

struct token {
  token_type type{token_type::invalid};
  std::string_view text{};
  std::uint32_t offset{0};
  std::uint32_t line{1};
  std::uint32_t column{1};

  constexpr std::string_view type_name() const noexcept {
    return token_type_to_str(type);
  }

  std::string to_str() const;
};

bool operator==(const token &lhs, const token &rhs) noexcept;

// for GTest
void PrintTo(const token &tok, std::ostream *os);

} // namespace pit::lexer
