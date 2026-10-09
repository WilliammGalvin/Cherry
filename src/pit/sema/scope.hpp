#pragma once

#include <cassert>
#include <cstddef>
#include <optional>
#include <span>
#include <vector>

namespace pit::sema {

template <typename T> class frame_stack {
public:
  void push_frame() { _frames.push_back(_entries.size()); }

  void pop_frame() {
    assert(!_frames.empty());
    _entries.erase(_entries.begin() +
                       static_cast<std::ptrdiff_t>(_frames.back()),
                   _entries.end());
    _frames.pop_back();
  }

  std::size_t depth() const noexcept { return _frames.size(); }

  void add(const T &entry) { _entries.push_back(entry); }

  std::span<const T> current() const noexcept {
    return {_entries.data() + _start_of(depth()),
            _entries.size() - _start_of(depth())};
  }

  std::span<const T> since_frame(std::size_t d) const noexcept {
    return {_entries.data() + _start_of(d), _entries.size() - _start_of(d)};
  }

  std::span<const T> all() const noexcept { return _entries; }

private:
  std::vector<T> _entries;
  std::vector<std::size_t> _frames;

  std::size_t _start_of(std::size_t d) const noexcept {
    return d == 0 || _frames.empty() ? 0 : _frames[d - 1];
  }
};

template <typename Key, typename Value> class scope_stack {
public:
  void push() { _stack.push_frame(); }

  void pop() { _stack.pop_frame(); }

  std::size_t depth() const noexcept { return _stack.depth(); }

  std::optional<Value> declare(const Key &key, const Value &value) {
    if (auto existing = _find(_stack.current(), key))
      return existing;

    _stack.add({key, value});
    return std::nullopt;
  }

  std::optional<Value> lookup(const Key &key) const noexcept {
    return _find(_stack.all(), key);
  }

  std::optional<Value> lookup_in_current(const Key &key) const noexcept {
    return _find(_stack.current(), key);
  }

private:
  struct entry {
    Key key;
    Value value;
  };

  frame_stack<entry> _stack;

  static std::optional<Value> _find(std::span<const entry> entries,
                                    const Key &key) {
    for (std::size_t i = entries.size(); i-- > 0;) {
      if (entries[i].key == key)
        return entries[i].value;
    }

    return std::nullopt;
  }
};

template <typename Stack> class frame_guard {
public:
  explicit frame_guard(Stack &stack) : _stack(stack) { _stack.push(); }

  ~frame_guard() { _stack.pop(); }

  frame_guard(const frame_guard &) = delete;
  frame_guard &operator=(const frame_guard &) = delete;

private:
  Stack &_stack;
};

} // namespace pit::sema
