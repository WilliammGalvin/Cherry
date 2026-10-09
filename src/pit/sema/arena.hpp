#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <limits>
#include <memory>
#include <new>
#include <span>
#include <type_traits>
#include <utility>

namespace pit::sema {

class arena_block {
public:
  explicit arena_block(std::size_t capacity_bytes)
      : _data(static_cast<std::byte *>(::operator new(capacity_bytes))),
        _capacity(capacity_bytes), _offset(0) {}

  ~arena_block() { ::operator delete(_data); }

  arena_block(const arena_block &) = delete;
  arena_block &operator=(const arena_block &) = delete;

  arena_block(arena_block &&) = delete;
  arena_block &operator=(arena_block &&) = delete;

  [[nodiscard]] void *allocate_bytes(std::size_t size,
                                     std::size_t align) noexcept {
    void *ptr = _data + _offset;
    std::size_t space = _capacity - _offset;
    if (!std::align(align, size, ptr, space))
      return nullptr;

    _offset =
        static_cast<std::size_t>(static_cast<std::byte *>(ptr) - _data) + size;
    return ptr;
  }

  arena_block *next() const { return _next; }
  void set_next(arena_block *next) { _next = next; }

private:
  std::byte *_data;
  std::size_t _capacity;
  std::size_t _offset{0};
  arena_block *_next{nullptr};
};

class arena {
public:
  explicit arena(std::size_t block_capacity_bytes)
      : _block_capacity(block_capacity_bytes) {}

  ~arena() {
    while (_head) {
      arena_block *nxt = _head->next();
      delete _head;
      _head = nxt;
    }
  }

  arena(const arena &) = delete;
  arena &operator=(const arena &) = delete;

  arena(arena &&) = delete;
  arena &operator=(arena &&) = delete;

  template <typename T, typename... Args>
    requires std::is_trivially_destructible_v<T>
  [[nodiscard]] T *allocate(Args &&...args) {
    void *ptr = _allocate_bytes(sizeof(T), alignof(T));
    assert(ptr);
    return new (ptr) T(std::forward<Args>(args)...);
  }

  template <typename T>
    requires std::is_default_constructible_v<T> &&
             std::is_trivially_destructible_v<T>
  [[nodiscard]] std::span<T> allocate_array(std::size_t n) {
    if (n == 0 || n > std::numeric_limits<std::size_t>::max() / sizeof(T))
      return {};

    void *ptr = _allocate_bytes(sizeof(T) * n, alignof(T));
    if (!ptr)
      return {};

    T *start = static_cast<T *>(ptr);
    std::uninitialized_value_construct_n(start, n);
    return std::span<T>(start, n);
  }

  template <typename T>
    requires std::is_copy_constructible_v<T> &&
             std::is_trivially_destructible_v<T>
  [[nodiscard]] std::span<T> allocate_copy(std::span<const T> items) {
    const std::size_t n = items.size();
    if (n == 0 || n > std::numeric_limits<std::size_t>::max() / sizeof(T))
      return {};

    void *ptr = _allocate_bytes(sizeof(T) * n, alignof(T));
    if (!ptr)
      return {};

    T *start = static_cast<T *>(ptr);
    std::uninitialized_copy(items.begin(), items.end(), start);
    return std::span<T>(start, n);
  }

private:
  std::size_t _block_capacity;
  arena_block *_head{nullptr};

  void _add_block(std::size_t min_bytes) {
    auto *block = new arena_block(std::max(_block_capacity, min_bytes));
    block->set_next(_head);
    _head = block;
  }

  [[nodiscard]] void *_allocate_bytes(std::size_t size, std::size_t align) {
    assert(size > 0 && align > 0 && (align & (align - 1)) == 0);

    if (_head) {
      if (void *ptr = _head->allocate_bytes(size, align))
        return ptr;
    }

    if (size > std::numeric_limits<std::size_t>::max() - align)
      return nullptr;

    _add_block(size + align);

    void *ptr = _head->allocate_bytes(size, align);
    assert(ptr && "fresh block must fit the request");
    return ptr;
  }
};

} // namespace pit::sema
