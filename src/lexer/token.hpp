#pragma once

#include "token_type.hpp"
#include <string>

namespace cherry::lexer {

struct token {
  token_type type;
  std::string value;

  token(token_type type, std::string value = "");

  [[nodiscard]] std::string to_str() const;
};

} // namespace cherry::lexer
