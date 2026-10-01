/// @file source_cursor.hpp
/// @brief Contains the definition of the source_cursor class, which is used to
///        navigate through the source code during the lexing process.
#pragma once

#include <concepts>
#include <cstddef>
#include <type_traits>

#include "pit/lexer/lex_error.hpp"

namespace pit::lexer {

/// @brief The source_cursor class is responsible for navigating through the
///        source code during the lexing process.
class source_cursor {
public:
  using position_type =
      lex_error::position_type; ///< Type used for representing the position of
                                ///< tokens in the source code.

  /// @brief Constructs a source_cursor object with the given source code.
  /// @param source The source code to be navigated.
  explicit constexpr source_cursor(std::string_view source) noexcept
      : _source(source) {}

  /// @brief Returns the character at the current position in the source code,
  ///        or the character at a specified number of characters ahead.
  /// @param ahead The number of characters ahead to peek. Defaults to 0 (the
  ///              current position).
  /// @return The character at the current position or the character at the
  ///         specified number of characters ahead, or '\0' if the position is
  ///         out of bounds.
  constexpr char peek(std::size_t ahead = 0) const noexcept {
    return _pos + ahead < _source.size() ? _source[_pos + ahead] : '\0';
  }

  /// @brief Advances the cursor by one character and returns the character that
  ///        was advanced over. If the character is a newline, it also updates
  ///        the line number and column number.
  /// @return The character that was advanced over.
  constexpr char advance() noexcept {
    const char c = _source[_pos++];
    if (c == '\n') {
      ++_line;
      _col = 1;
    } else {
      ++_col;
    }

    return c;
  }

  /// @brief Advances the cursor by a specified number of characters.
  /// @param count The number of characters to advance the cursor by.
  constexpr void advance(std::size_t count) noexcept {
    for (std::size_t i = 0; i < count; ++i)
      advance();
  }

  /// @brief Checks if the character at the current position matches the
  ///        expected character. If it does, the cursor is advanced by one
  ///        character.
  /// @param expected The expected character to match.
  /// @return true if the character at the current position matches the expected
  ///         character, false otherwise.
  constexpr bool match(char expected) noexcept {
    if (peek() != expected)
      return false;

    advance();
    return true;
  }

  /// @brief Advances the cursor while the predicate returns true for the
  ///        character at the current position.
  /// @tparam Predicate A predicate that takes a character and returns a boolean
  ///                   indicating whether to continue advancing.
  template <std::predicate<char> Predicate>
  constexpr void advance_while(Predicate pred) noexcept(
      std::is_nothrow_invocable_v<Predicate &, char>) {
    while (!at_end() && pred(peek())) {
      advance();
    }
  }

  template <std::predicate<char> Predicate>
  constexpr bool is_peek(Predicate pred, std::size_t lookahead = 0) const
      noexcept(std::is_nothrow_invocable_v<Predicate &, char>) {
    return !at_end(lookahead) && pred(peek(lookahead));
  }

  /// @brief Checks if the source code starting from the current position starts
  ///        with the given prefix.
  /// @param prefix The prefix to check for at the current position.
  /// @return true if the source code starting from the current position starts
  ///         with the given prefix, false otherwise.
  constexpr bool starts_with(std::string_view prefix) const noexcept {
    return _source.substr(_pos).starts_with(prefix);
  }

  /// @brief Checks if the lexer has reached the end of the source code.
  /// @param offset An optional offset to check for the end of the source code
  ///               after the current position. Defaults to 0 (the current
  ///               position).
  /// @return true if the lexer has reached the end of the source code, false
  ///         otherwise.
  constexpr bool at_end(std::size_t offset = 0) const noexcept {
    return _pos + offset >= _source.size();
  }

  /// @brief Returns a slice of the source code from the specified start
  /// position
  ///        to the specified end position.
  /// @param start The starting position of the slice.
  /// @param end The ending position of the slice.
  /// @return A string_view representing the slice of the source code.
  constexpr std::string_view slice(std::size_t start,
                                   std::size_t end) const noexcept {
    return _source.substr(start, end - start);
  }

  /// @brief Resets the cursor's position, line number, and column number to the
  ///        beginning of the source code.
  constexpr void reset() noexcept {
    _pos = 0;
    _line = 1;
    _col = 1;
  }

  /// @brief Returns the current position in the source code.
  /// @return The current position in the source code.
  constexpr position_type offset() const noexcept { return _pos; }

  /// @brief Returns the current line number in the source code.
  /// @return The current line number in the source code.
  constexpr position_type line() const noexcept { return _line; }

  /// @brief Returns the line number at the specified position in the source
  ///        code.
  /// @param pos The position in the source code for which to retrieve the line
  ///            number.
  /// @return The line number at the specified position in the source code.
  constexpr position_type line_at(position_type pos) const noexcept {
    position_type line = 1;
    for (position_type i = 0; i < pos && i < _source.size(); ++i) {
      if (_source[i] == '\n')
        ++line;
    }

    return line;
  }

  /// @brief Returns the current column number in the source code.
  /// @return The current column number in the source code.
  constexpr position_type column() const noexcept { return _col; }

  /// @brief Returns the column number at the specified position in the source
  ///        code.
  /// @param pos The position in the source code for which to retrieve the
  ///            column number.
  /// @return The column number at the specified position in the source code.
  constexpr position_type column_at(position_type pos) const noexcept {
    position_type col = 1;
    for (position_type i = 0; i < pos && i < _source.size(); ++i) {
      if (_source[i] == '\n')
        col = 1;
      else
        ++col;
    }

    return col;
  }

private:
  std::string_view _source;
  position_type _pos{0};
  position_type _line{1};
  position_type _col{1};
};

} // namespace pit::lexer
