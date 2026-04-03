#include "lexer.hpp"
#include "token_type.hpp"

#include <fstream>
#include <unordered_map>

namespace cherry::lexer {

static constexpr bool is_alpha_or_underscore(const char c) noexcept {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static constexpr bool is_alphanumeric_or_underscore(const char c) noexcept {
  return is_alpha_or_underscore(c) || (c >= '0' && c <= '9');
}

char lexer::peek(const std::size_t pos) const noexcept {
  if (index_ + pos >= line_source_.length())
    return '\0';
  return line_source_.at(index_ + pos);
}

std::expected<char, lex_error> lexer::consume() noexcept {
  if (index_ >= line_source_.length())
    return std::unexpected{lex_error::unexpected_end_of_input};

  return line_source_[index_++];
}

std::expected<void, lex_error> lexer::advance(const std::size_t pos) noexcept {
  if (index + pos > line_source.length())
    std::unexpected{lex_error::unexpected_end_of_input};

  index_ += pos;
  return {};
}

bool lexer::is_empty() const noexcept {
  return index_ >= line_source_.length();
}

bool lexer::match_front(const std::string &str) const noexcept {
  return std::string_view(line_source_).substr(index_).starts_with(str);
}

bool lexer::match_keyword() {
  if (!is_alpha_or_underscore(peek()))
    return false;

  const std::unordered_map<std::string, token_type> keywords = {
      {"bool", token_type::kw_bool},
      {"break", token_type::kw_break},
      {"const", token_type::kw_const},
      {"continue", token_type::kw_continue},
      {"else", token_type::kw_else},
      {"false", token_type::boolean_literal_false},
      {"float", token_type::kw_float},
      {"func", token_type::kw_func},
      {"if", token_type::kw_if},
      {"int", token_type::kw_int},
      {"loop", token_type::kw_for},
      {"private", token_type::kw_private},
      {"public", token_type::kw_public},
      {"return", token_type::kw_return},
      {"string", token_type::kw_string},
      {"true", token_type::boolean_literal_true},
      {"void", token_type::kw_void},
      {"while", token_type::kw_while},
  };

  std::string longest;
  for (const auto &[key, value] : keywords)
    if (match_front(key) && key.length() > longest.length())
      longest = key;

  if (longest.empty())
    return false;

  advance(longest.length());
  tokens_.emplace_back(keywords.at(longest), longest);
  return true;
}

bool lexer::match_symbol() {
  const std::vector<std::pair<std::string, token_type>> symbols = {
      {"!=", token_type::bang_equal},    {"%", token_type::percent},
      {"%=", token_type::percent_equal}, {"&&", token_type::logical_and},
      {"(", token_type::left_paren},     {")", token_type::right_paren},
      {"*", token_type::star},           {"*=", token_type::star_equal},
      {"+", token_type::plus},           {"+=", token_type::plus_equal},
      {",", token_type::comma},          {"-", token_type::minus},
      {"-=", token_type::minus_equal},   {"/", token_type::slash},
      {"/=", token_type::slash_equal},   {":", token_type::colon},
      {";", token_type::semi_colon},     {"<", token_type::less},
      {"<=", token_type::less_equal},    {"=", token_type::equal},
      {"==", token_type::double_equal},  {">", token_type::greater},
      {">=", token_type::greater_equal}, {"[", token_type::left_bracket},
      {"]", token_type::right_bracket},  {"{", token_type::left_brace},
      {"||", token_type::logical_or},    {"}", token_type::right_brace},
      {"!", token_type::bang},
  };

  for (const auto &[key, value] : symbols) {
    if (!match_front(key))
      continue;

    advance(key.length());
    tokens_.emplace_back(value, key);
    return true;
  }

  return false;
}

bool lexer::match_identifier() {
  std::string buffer;
  if (!is_alpha_or_underscore(peek()))
    return false;

  buffer += consume();
  while (!is_empty() && is_alphanumeric_or_underscore(peek())) {
    buffer += consume();
  }

  tokens_.emplace_back(token_type::identifier, buffer);
  return true;
}

bool lexer::match_number_literal() {
  std::string buffer;
  bool is_float = false;

  while (!is_empty()) {
    if (std::isdigit(peek())) {
      buffer += consume();
      continue;
    }

    if (peek() == '.' && !is_float) {
      if (buffer.empty())
        buffer += '0';

      buffer += consume();
      is_float = true;
      continue;
    }

    if ((peek() == 'f' || peek() == 'F') && !buffer.empty()) {
      is_float = true;
      advance(1);
      break;
    }

    break;
  }

  if (!buffer.empty()) {
    token_type type =
        is_float ? token_type::float_literal : token_type::integer_literal;
    tokens_.emplace_back(type, buffer);
    return true;
  }

  return false;
}

bool lexer::match_string_literal() {
  std::string buffer;

  if (peek() != '"') {
    return false;
  }

  advance(1);

  while (!is_empty()) {
    if (peek() == '"') {
      advance(1);
      tokens_.emplace_back(token_type::string_literal, buffer);
      return true;
    }

    buffer += consume();
  }

  return false;
}

bool lexer::match_directive() {
  if (peek() != '@')
    return false;

  advance();
  std::string directive;
  while (!is_empty() && is_alphanumeric_or_underscore(peek())) {
    directive += consume();
  }

  tokens_.emplace_back(token_type::directive, directive);
  return true;
}

std::expected<void, lex_error> lexer::lex_line() {
  while (!is_empty()) {
    if (std::isspace(peek())) {
      advance(1);
      continue;
    }

    if (match_front("*/")) {
      in_block_comment_ = false;
      advance(2);
      continue;
    }

    if (in_block_comment_) {
      advance(1);
      continue;
    }

    if (match_front("/*")) {
      in_block_comment_ = true;
      advance(2);
      continue;
    }

    if (match_front("//")) {
      line_source_.clear();
      break;
    }

    if (match_symbol() || match_keyword() || match_number_literal() ||
        match_string_literal() || match_identifier() || match_directive())
      continue;

    return std::unexpected{lex_error::unidentifiable_token};
  }

  return {};
}

std::vector<token> lexer::lex_file(std::string_view file_name) {
  std::ifstream file(std::string{file_name});
  if (!file.is_open())
    throw std::runtime_error("Could not open source file.");

  while (std::getline(file, line_source_)) {
    lex_line();
    tokens_.emplace_back(token_type::line_end);
    line_source_.clear();
    index_ = 0;
  }

  tokens_.emplace_back(token_type::eof);
  return tokens_;
}

} // namespace cherry::lexer
