#include "intern_table.hpp"

#include <cassert>
#include <cstddef>
#include <span>
#include <string_view>

namespace pit::sema {

intern_table::intern_table(arena &strings) : _arena(strings) {
  _strings.reserve(1024);
  _map.reserve(1024);
  _strings.emplace_back(); // reserve id = 0 as invalid
}

name_id intern_table::intern(std::string_view text) {
  if (text.empty())
    return name_id::none;

  const auto it = _map.find(text);
  if (it != _map.end())
    return static_cast<name_id>(it->second);

  auto copy = _arena.allocate_copy<char>(std::span<const char>(text));
  const std::string_view owned(copy.data(), copy.size());

  const auto id = static_cast<std::uint32_t>(_strings.size());
  _strings.push_back(owned);
  _map.emplace(owned, id);
  return static_cast<name_id>(id);
}

std::string_view intern_table::text(name_id name) const {
  const auto i = static_cast<std::size_t>(name);
  assert(i < _strings.size());
  return _strings[i];
}

} // namespace pit::sema
