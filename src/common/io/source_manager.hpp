#pragma once

#include <cstddef>
#include <expected>
#include <filesystem>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace common::io {

enum class source_manager_error {
  source_not_found,
  source_too_large,
  cant_read_size,
  cant_read_source,
};

using source_id = std::uint32_t;

struct source_info {
  std::string_view path;
  std::string_view text;
};

class source_manager {
public:
  std::expected<source_id, source_manager_error>
  add_source(const std::filesystem::path &path);

  source_info get_source(source_id id) const {
    if (id >= _sources.size())
      return {};

    return _sources[id];
  }

private:
  static constexpr std::size_t _block_size{64 * 1024}; // 64 KB

  struct block {
    std::unique_ptr<char[]> data;
    std::size_t used{0};
  };

  std::vector<block> _blocks;
  std::vector<std::unique_ptr<char[]>> _large;
  std::vector<source_info> _sources;
  std::unordered_map<std::string_view, source_id> _by_path;

  char *_allocate(std::size_t n);
};

} // namespace common::io
