/// @file lexer.hpp
/// @brief Contains the definition of the lexer class, which is responsible for
///        tokenizing the source code into a sequence of tokens.
#pragma once

#include <expected>
#include <string_view>
#include <vector>

#include "pit/lexer/lex_error.hpp"
#include "pit/lexer/source_cursor.hpp"
#include "pit/lexer/token.hpp"

namespace pit::lexer {

/// @brief Represents the result of the lexing process, which can either be a
///        vector of tokens or a lex_error.

/// @brief The lexer class is responsible for tokenizing the source code into a
///        sequence of tokens. It processes the input source code and produces
///        a vector of tokens or a lex_error if an error occurs during the
///        lexing process.
class lexer {
public:
  /// @brief Constructs a lexer object with the given source code.
  /// @param source The source code to be tokenized.
  explicit lexer(std::string_view source) noexcept : _cursor(source) {}

  /// @brief Tokenizes the source code and returns a lex_result, which can be
  ///        either a vector of tokens or a lex_error if an error occurs during
  ///        the lexing process.
  /// @return A lex_result containing either a vector of tokens or a lex_error.
  std::expected<std::vector<token>, lex_error> lex();

private:
  using lex_result = std::expected<void, lex_error>;
  using position_type = source_cursor::position_type;

  source_cursor _cursor;
  std::vector<token> _tokens{};

  /// @brief Lexes an identifier or keyword from the source code. If an error
  ///        occurs during this process, it returns a lex_error.
  /// @return A lex_result indicating success or failure.
  lex_result _lex_identifier_or_keyword();

  /// @brief Lexes a number from the source code. If an error occurs during this
  ///        process, it returns a lex_error.
  /// @return A lex_result indicating success or failure.
  lex_result _lex_number();

  /// @brief Lexes a directive from the source code. If an error occurs during
  ///        this process, it returns a lex_error.
  /// @return A lex_result indicating success or failure.
  lex_result _lex_directive();

  /// @brief Lexes a string literal from the source code. If an error occurs
  ///        during this process, it returns a lex_error.
  /// @return A lex_result indicating success or failure.
  lex_result _lex_string_literal();

  /// @brief Lexes a symbol (operator or punctuation) from the source code. If
  /// an
  ///        error occurs during this process, it returns a lex_error.
  /// @return A lex_result indicating success or failure.
  lex_result _lex_symbol();

  /// @brief Pushes a token of the specified type onto the token vector. The
  ///        token is created using the specified contents and the current
  ///        position of the source cursor.
  /// @param type The type of the token to be pushed.
  /// @param contents The contents of the token to be pushed.
  void _push_token(token_type type, std::string_view contents) {
    _tokens.emplace_back(type, contents, _cursor.offset(), _cursor.line(),
                         _cursor.column());
  }

  /// @brief Pushes a token of the specified type onto the token vector. The
  ///        token is created using the specified start and stop positions in
  ///        the source code.
  /// @param type The type of the token to be pushed.
  /// @param start The starting position of the token in the source code.
  /// @param stop The stopping position of the token in the source code.
  void _push_token(token_type type, position_type start, position_type stop) {
    const auto contents = _cursor.slice(start, stop);
    _tokens.emplace_back(type, contents, start, _cursor.line_at(start),
                         _cursor.column_at(start));
  }

  /// @brief Pushes a token of the specified type onto the token vector. The
  ///        token is created using the specified start position and the current
  ///        position of the source cursor.
  /// @param type The type of the token to be pushed.
  /// @param start The starting position of the token in the source code.
  void _push_token(token_type type, position_type start) {
    _push_token(type, start, _cursor.offset());
  }

  /// @brief Pushes a token of the specified type onto the token vector. The
  ///        token is created using the current position of the source cursor.
  /// @param type The type of the token to be pushed.
  void _push_token(token_type type) {
    _push_token(type, _cursor.offset(), _cursor.offset());
  }

  /// @brief Resets the lexer to its initial state, clearing the token vector
  ///        and resetting the source cursor to the beginning of the source
  ///        code.
  void _reset() noexcept {
    _cursor.reset();
    _tokens.clear();
  }

  /// @brief Skips comments and whitespace in the source code. If an error
  ///        occurs during this process, it returns a lex_error.
  /// @return An expected<void, lex_error> indicating success or failure.
  lex_result _skip_comments_and_whitespace();

  /// @brief Creates a lex_error at the current cursor position with the given
  ///        lex_error_kind.
  /// @param kind The kind of lexical error to create.
  /// @return A lex_error object representing the lexical error at the current
  ///         cursor position.
  lex_error _error_here(lex_error_kind kind) const noexcept {
    return lex_error{kind, _cursor.offset(), _cursor.line(), _cursor.column()};
  }
};

} // namespace pit::lexer
