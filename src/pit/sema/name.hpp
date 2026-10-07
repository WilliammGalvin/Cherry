#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>

namespace pit::sema {

enum class name_id : std::uint32_t { none = 0xFFFFFFFF };

class name_interner {
public:
  name_id intern(std::string_view name) {
    const auto it = _lookup.find(name);
    if (it != _lookup.end())
      return it->second;

    const std::string &stored = _names.emplace_back(name);
    const auto id = static_cast<name_id>(_names.size() - 1);
    _lookup.emplace(stored, id);
    return id;
  }

  name_id find(std::string_view name) const {
    const auto it = _lookup.find(name);
    return it == _lookup.end() ? name_id::none : it->second;
  }

  std::string_view name(name_id id) const {
    return _names[static_cast<std::size_t>(id)];
  }

  std::size_t size() const noexcept { return _names.size(); }

private:
  std::deque<std::string> _names;
  std::unordered_map<std::string_view, name_id> _lookup;
};

} // namespace pit::sema
