#pragma once

#include <common/lexing/source_file.hpp>
#include <cstdint>
#include <deque>
#include <expected>
#include <filesystem>
#include <string>
#include <unordered_map>

namespace common::lexing {

using source_file_id = std::uint32_t;

enum class load_error {
  not_found,
  unreadable,
  too_large,
};

class source_manager {
public:
  std::expected<source_file_id, load_error>
  add_source(const std::filesystem::path &path);

  const source_file &get(source_file_id id) const { return _files[id]; }

private:
  std::deque<source_file> _files;
  std::unordered_map<std::string, source_file_id> _ids;
};

} // namespace common::lexing
