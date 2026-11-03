#pragma once

#include <cstdint>
#include <expected>
#include <filesystem>
#include <fstream>
#include <ios>
#include <system_error>

namespace common::io {

enum class source_read_error {
  file_not_found,
  failed_to_open_file,
  failed_to_read_file,
};

inline std::expected<std::uintmax_t, source_read_error>
source_file_size(const std::filesystem::path &path) {
  std::error_code ec;
  auto size = std::filesystem::file_size(path, ec);
  if (ec == std::errc::no_such_file_or_directory)
    return std::unexpected(source_read_error::file_not_found);

  if (ec)
    return std::unexpected(source_read_error::failed_to_read_file);

  return size;
};

inline std::expected<std::size_t, source_read_error>
read_source_file(const std::filesystem::path &path, std::span<char> out) {
  std::ifstream in(path, std::ios::binary);
  if (!in)
    return std::unexpected(source_read_error::failed_to_open_file);

  in.read(out.data(), static_cast<std::streamsize>(out.size()));
  if (in.bad())
    return std::unexpected(source_read_error::failed_to_read_file);

  return static_cast<std::size_t>(in.gcount());
}

} // namespace common::io
