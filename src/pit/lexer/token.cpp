#include "pit/lexer/token.hpp"

#include <ostream>

namespace pit::lexer {

std::string token::to_str() const {
  auto str = std::string{token_type_to_str(type)};
  if (!text.empty()) {
    str += '(';
    str += text;
    str += ')';
  }
  return str;
}

bool operator==(const token &lhs, const token &rhs) noexcept {
  return lhs.type == rhs.type && lhs.text == rhs.text &&
         lhs.offset == rhs.offset && lhs.line == rhs.line &&
         lhs.column == rhs.column;
}

void PrintTo(const token &tok, std::ostream *os) {
  *os << tok.to_str() << " at " << tok.line << ':' << tok.column << " (offset "
      << tok.offset << ')';
}

} // namespace pit::lexer
