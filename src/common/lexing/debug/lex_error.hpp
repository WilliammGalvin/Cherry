#pragma once

#include <common/lexing/lex_error.hpp>
#include <cstddef>
#include <format>
#include <string>
#include <string_view>
#include <utility>

namespace common::debug::lexing {

using namespace common::lexing;

constexpr std::string_view to_str(lex_error_kind kind) noexcept {
  switch (kind) {
  case lex_error_kind::unterminated_string:
    return "unterminated string literal";
  case lex_error_kind::unterminated_block_comment:
    return "unterminated block comment";
  case lex_error_kind::invalid_escape_sequence:
    return "invalid escape sequence";
  case lex_error_kind::unexpected_character:
    return "unexpected character";
  }

  std::unreachable();
}

constexpr std::string_view to_label(lex_error_kind kind) noexcept {
  switch (kind) {
  case lex_error_kind::unterminated_string:
    return "string starts here but is never closed";
  case lex_error_kind::unterminated_block_comment:
    return "comment starts here but is never closed";
  case lex_error_kind::invalid_escape_sequence:
    return "unknown escape here";
  case lex_error_kind::unexpected_character:
    return "not valid in this position";
  }

  std::unreachable();
}

namespace detail {

inline std::string_view line_at(std::string_view src, std::size_t line) {
  std::size_t start = 0;
  for (std::size_t i = 1; i < line; ++i) {
    auto nl = src.find('\n', start);
    if (nl == std::string_view::npos)
      return {};

    start = nl + 1;
  }

  auto end = src.find('\n', start);
  auto sv =
      src.substr(start, end == std::string_view::npos ? std::string_view::npos
                                                      : end - start);

  if (!sv.empty() && sv.back() == '\r')
    sv.remove_suffix(1);

  return sv;
}

inline std::size_t digits(std::size_t n) {
  std::size_t count = 1;
  while (n >= 10) {
    n /= 10;
    ++count;
  }

  return count;
}

} // namespace detail

inline std::string to_str(const lex_error &err, std::string_view source,
                          std::string_view filename) {
  const auto line = static_cast<std::size_t>(err.line);
  const auto col = static_cast<std::size_t>(err.column);

  const auto text = detail::line_at(source, line);
  const auto w = detail::digits(line);
  const std::string gutter(w, ' ');

  std::string pad;
  for (std::size_t i = 0; i + 1 < col && i < text.size(); ++i)
    pad += (text[i] == '\t') ? '\t' : ' ';

  if (col > text.size() + 1)
    pad.append(col - 1 - text.size(), ' ');

  return std::format("error: {}\n"
                     "{} --> {}:{}:{}\n"
                     "{} |\n"
                     "{} | {}\n"
                     "{} | {}^ {}\n"
                     "{} |\n",
                     to_str(err.kind), gutter, filename, line, col, gutter,
                     line, text, gutter, pad, to_label(err.kind), gutter);
}

inline std::string to_str(std::span<const lex_error> errs,
                          std::string_view source, std::string_view filename) {
  std::string out;
  for (const auto &e : errs) {
    out += to_str(e, source, filename);
    out += '\n';
  }

  out += std::format("error: aborting due to {} lexing error{}\n", errs.size(),
                     errs.size() == 1 ? "" : "s");
  return out;
}

} // namespace common::debug::lexing
