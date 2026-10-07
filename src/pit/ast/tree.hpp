#pragma once

#include "node.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <type_traits>
#include <vector>

namespace pit::ast {

template <typename Id> class id_view {
public:
  id_view(const std::uint32_t *data, std::size_t size) noexcept
      : _data(data), _size(size) {}

  Id operator[](std::size_t index) const noexcept {
    return static_cast<Id>(_data[index]);
  }

  std::size_t size() const noexcept { return _size; }

  bool empty() const noexcept { return _size == 0; }

  struct iterator {
    const std::uint32_t *ptr;

    Id operator*() const noexcept { return static_cast<Id>(*ptr); }

    iterator &operator++() noexcept {
      ++ptr;
      return *this;
    }

    bool operator!=(const iterator &other) const noexcept {
      return ptr != other.ptr;
    }
  };

  iterator begin() const noexcept { return {_data}; }

  iterator end() const noexcept { return {_data + _size}; }

private:
  const std::uint32_t *_data;
  std::size_t _size;
};

class tree {
public:
  explicit tree(std::size_t token_count = 0) {
    const std::size_t estimate = token_count / 2 + 1;
    _tags.reserve(estimate);
    _main_tokens.reserve(estimate);
    _data.reserve(estimate);
    _flags.reserve(estimate);
    _extra.reserve(estimate);

    add(node_tag::root, token_id::none);
  }

  node_tag tag(node_id id) const noexcept { return _tags[_at(id)]; }

  token_id main_token(node_id id) const noexcept {
    return _main_tokens[_at(id)];
  }

  node_flags flags(node_id id) const noexcept { return _flags[_at(id)]; }

  std::uint32_t node_count() const noexcept {
    return static_cast<std::uint32_t>(_tags.size());
  }

  node_id lhs(node_id id) const noexcept { return node_id{_data[_at(id)].lhs}; }

  node_id rhs(node_id id) const noexcept { return node_id{_data[_at(id)].rhs}; }

  extra_id lhs_extra(node_id id) const noexcept {
    return extra_id{_data[_at(id)].lhs};
  }

  extra_id rhs_extra(node_id id) const noexcept {
    return extra_id{_data[_at(id)].rhs};
  }

  sub_range data_range(node_id id) const noexcept {
    const node_data d = _data[_at(id)];
    return {static_cast<extra_id>(d.lhs), static_cast<extra_id>(d.rhs)};
  }

  id_view<node_id> items() const noexcept {
    return range<node_id>(data_range(node_id{0}));
  }

  node_id add(node_tag tag, token_id main, node_data data = {},
              node_flags flags = 0) {
    const auto id = static_cast<node_id>(_tags.size());
    _tags.push_back(tag);
    _main_tokens.push_back(main);
    _data.push_back(data);
    _flags.push_back(flags);
    return id;
  }

  void set_data(node_id id, node_data data) noexcept { _data[_at(id)] = data; }

  void set_tag(node_id id, node_tag tag) noexcept { _tags[_at(id)] = tag; }

  void add_flags(node_id id, node_flags flags) noexcept {
    _flags[_at(id)] |= flags;
  }

  template <typename Id> sub_range add_range(std::span<const Id> ids) {
    const auto start = _extra_size();
    for (Id id : ids)
      _extra.push_back(static_cast<std::uint32_t>(id));

    return {static_cast<extra_id>(start), static_cast<extra_id>(_extra.size())};
  }

  template <typename Id> id_view<Id> range(sub_range r) const noexcept {
    const auto start = static_cast<std::uint32_t>(r.start);
    const auto end = static_cast<std::uint32_t>(r.end);
    return {_extra.data() + start, end - start};
  }

  template <typename T> extra_id add_extra(const T &value) {
    static_assert(std::is_trivially_copyable_v<T>);
    static_assert(sizeof(T) % sizeof(std::uint32_t) == 0);

    const auto start = _extra_size();
    _extra.resize(start + sizeof(T) / sizeof(std::uint32_t));
    std::memcpy(_extra.data() + start, &value, sizeof(T));
    return static_cast<extra_id>(start);
  }

  template <typename T> [[nodiscard]] T extra(extra_id where) const noexcept {
    static_assert(std::is_trivially_copyable_v<T>);

    T value;
    std::memcpy(&value, _extra.data() + static_cast<std::uint32_t>(where),
                sizeof(T));
    return value;
  }

private:
  std::vector<node_tag> _tags;
  std::vector<token_id> _main_tokens;
  std::vector<node_data> _data;
  std::vector<node_flags> _flags;
  std::vector<std::uint32_t> _extra;

  static std::uint32_t _at(node_id id) noexcept {
    return static_cast<std::uint32_t>(id);
  }

  std::uint32_t _extra_size() const noexcept {
    return static_cast<std::uint32_t>(_extra.size());
  }
};

} // namespace pit::ast
