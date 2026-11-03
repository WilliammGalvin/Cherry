#include <common/io/read_source_file.hpp>
#include <common/io/source_manager.hpp>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <expected>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <system_error>

namespace common::io {

std::expected<source_id, source_manager_error>
source_manager::add_source(const std::filesystem::path &path) {
  std::error_code ec;
  auto canon = std::filesystem::canonical(path, ec);
  if (ec)
    return std::unexpected(source_manager_error::source_not_found);

  std::string key = canon.string();
  if (auto it = _by_path.find(key); it != _by_path.end())
    return it->second;

  auto size = source_file_size(canon);
  if (!size)
    return std::unexpected(source_manager_error::cant_read_size);

  if (*size >= std::numeric_limits<std::uint32_t>::max())
    return std::unexpected(source_manager_error::source_too_large);

  char *text = _allocate(*size + 1);
  auto read_bytes =
      read_source_file(canon, {text, static_cast<std::size_t>(*size)});
  if (!read_bytes)
    return std::unexpected(source_manager_error::cant_read_source);
  text[*read_bytes] = '\0';

  char *pmem = _allocate(key.size() + 1);
  std::memcpy(pmem, key.c_str(), key.size() + 1);
  std::string_view path_sv(pmem, key.size());

  auto id = static_cast<source_id>(_sources.size());
  _sources.push_back({path_sv, {text, static_cast<std::size_t>(*read_bytes)}});
  _by_path.emplace(path_sv, id);

  return id;
}

char *source_manager::_allocate(std::size_t n) {
  if (n > _block_size) {
    _large.push_back(std::make_unique_for_overwrite<char[]>(n));
    return _large.back().get();
  }

  if (_blocks.empty() || _block_size - _blocks.back().used < n)
    _blocks.push_back({std::make_unique_for_overwrite<char[]>(_block_size), 0});

  block &b = _blocks.back();
  char *ptr = b.data.get() + b.used;
  b.used += n;

  return ptr;
}

} // namespace common::io
