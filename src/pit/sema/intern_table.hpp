#pragma once

#include "arena.hpp"
#include "name.hpp"

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace pit::sema {

class intern_table {
public:
  explicit intern_table(arena &strings);

  intern_table(const intern_table &) = delete;
  intern_table &operator=(const intern_table &) = delete;

  intern_table(intern_table &&) = delete;
  intern_table &operator=(intern_table &&) = delete;

  name_id intern(std::string_view text);

  std::string_view text(name_id name) const;

  std::size_t size() const { return _strings.size() - 1; }

private:
  arena &_arena;
  std::unordered_map<std::string_view, std::uint32_t> _map;
  std::vector<std::string_view> _strings;
};

} // namespace pit::sema
