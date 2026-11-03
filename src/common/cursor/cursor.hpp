#pragma once

#include <concepts>
#include <cstddef>
#include <span>
#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace common::cursor {

template <typename T>
concept char_type = std::same_as<T, char> || std::same_as<T, wchar_t> ||
                    std::same_as<T, char16_t> || std::same_as<T, char32_t>;

namespace detail {

template <typename T> struct location_tracker {
  constexpr void update(const T &) noexcept {}
};

template <char_type T> struct location_tracker<T> {
  std::size_t line{1};
  std::size_t column{1};

  constexpr void update(const T &c) noexcept {
    if (c == '\n') {
      ++line;
      column = 1;
    } else {
      ++column;
    }
  }
};

} // namespace detail

template <typename T> class cursor {
public:
  using position_type = std::size_t;

  explicit constexpr cursor(std::span<const T> source) noexcept
      : _source(source) {}

  constexpr const T &peek(std::size_t ahead = 0) const {
    if (at_end(ahead))
      throw std::out_of_range("Cursor peek out of range");

    return _source[_pos + ahead];
  }

  constexpr const T *try_peek(std::size_t ahead = 0) const noexcept {
    return at_end(ahead) ? nullptr : &_source[_pos + ahead];
  }

  constexpr const T &advance() noexcept {
    const T &value = _source[_pos];
    _loc.update(value);
    ++_pos;
    return value;
  }

  constexpr void advance(std::size_t count) noexcept {
    while (count-- > 0 && !at_end())
      advance();
  }

  constexpr bool
  match(const T &expected) noexcept(noexcept(expected != expected))
    requires std::equality_comparable<T>
  {
    if (at_end() || _source[_pos] != expected)
      return false;

    advance();
    return true;
  }

  template <std::predicate<const T &> Predicate>
  constexpr void advance_while(Predicate pred) noexcept(
      std::is_nothrow_invocable_v<Predicate &, const T &>) {
    while (!at_end() && pred(_source[_pos]))
      advance();
  }

  template <std::predicate<const T &> Predicate>
  constexpr bool is_peek(Predicate pred, std::size_t lookahead = 0) const
      noexcept(std::is_nothrow_invocable_v<Predicate &, const T &>) {
    return !at_end(lookahead) && pred(_source[_pos + lookahead]);
  }

  constexpr bool at_end(std::size_t offset = 0) const noexcept {
    return _pos + offset >= _source.size();
  }

  constexpr position_type offset() const noexcept { return _pos; }

  // char only helpers

  constexpr std::size_t line() const noexcept
    requires char_type<T>
  {
    return _loc.line;
  }

  constexpr std::size_t column() const noexcept
    requires char_type<T>
  {
    return _loc.column;
  }

  constexpr bool starts_with(std::string_view prefix) const noexcept
    requires char_type<T>
  {
    return _view().substr(_pos).starts_with(prefix);
  }

  constexpr std::string_view slice(std::size_t start,
                                   std::size_t end) const noexcept
    requires char_type<T>
  {
    if (start > end || end > _source.size())
      return {};

    return std::string_view{_source.data() + start, end - start};
  }

private:
  std::span<const T> _source;
  position_type _pos{0};
  [[no_unique_address]] detail::location_tracker<T> _loc{};

  constexpr std::string_view _view() const noexcept
    requires char_type<T>
  {
    return std::string_view{_source.data(), _source.size()};
  }
};

} // namespace common::cursor
