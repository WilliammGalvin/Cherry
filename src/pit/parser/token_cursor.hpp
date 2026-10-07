#pragma once

#include "pit/lexer/token.hpp"
#include "pit/lexer/token_type.hpp"

#include <cassert>
#include <cstddef>
#include <span>

namespace pit::parser {

class token_cursor {
public:
  using token = lexer::token;
  using token_type = lexer::token_type;
  using token_span = std::span<const token>;
  using position_type = std::size_t;

  explicit token_cursor(token_span tokens) : _tokens{tokens} {
    assert(!tokens.empty() && tokens.back().type == token_type::eof &&
           "token_cursor expects a valid token span");
  }

  const token &peek(std::size_t ahead = 0) const {
    const auto i = _pos + ahead;
    return i < _tokens.size() ? _tokens[i] : _tokens.back();
  }

  const token &next() {
    const token &t = peek();
    if (_pos + 1 < _tokens.size())
      ++_pos;

    return t;
  }

  bool check(token_type type) const { return peek().type == type; }

  bool match(token_type type) {
    if (!check(type))
      return false;

    next();
    return true;
  }

  bool at_end() const { return _pos + 1 >= _tokens.size(); }

  position_type pos() const { return _pos; }
  token_span tokens() const { return _tokens; }

private:
  token_span _tokens;
  position_type _pos{0};
};

} // namespace pit::parser
