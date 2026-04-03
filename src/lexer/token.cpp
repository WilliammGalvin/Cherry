#include "token.hpp"
#include "token_type.hpp"

namespace cherry::lexer {

token::token(token_type type, std::string value)
    : type(type), value(std::move(value)) {}

std::string token::to_str() const {
  auto str = std::string{token_type_to_str(type)};

  if (!value.empty()) {
    str += '(';
    str += value;
    str += ')';
  }

  return str;
}

} // namespace cherry::lexer
