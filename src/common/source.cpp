#include "common/source.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <utility>

namespace cherry::common {

source_file::source_file(std::string path, std::string contents)
    : path_(std::move(path)), contents_(std::move(contents)) {
  line_starts_.push_back(0);
  for (std::uint32_t i = 0; i < contents_.size(); ++i)
    if (contents_[i] == '\n')
      line_starts_.push_back(i + 1);
}

std::expected<source_file, std::string>
source_file::load(std::string_view path) {
  std::ifstream file{std::string{path}, std::ios::binary};
  if (!file)
    return std::unexpected{"could not open source file: " + std::string{path}};

  std::ostringstream buffer;
  buffer << file.rdbuf();
  if (file.bad())
    return std::unexpected{"error reading source file: " + std::string{path}};

  return source_file{std::string{path}, std::move(buffer).str()};
}

source_file source_file::from_string(std::string path, std::string contents) {
  return source_file{std::move(path), std::move(contents)};
}

source_file::position
source_file::position_of(std::uint32_t offset) const noexcept {
  const auto it = std::ranges::upper_bound(line_starts_, offset);
  const auto index =
      static_cast<std::uint32_t>(std::distance(line_starts_.begin(), it) - 1);
  return position{index + 1, offset - line_starts_[index] + 1};
}

std::string_view source_file::line_text(std::uint32_t line) const noexcept {
  if (line == 0 || line > line_starts_.size())
    return {};

  const std::uint32_t start = line_starts_[line - 1];
  const std::uint32_t end = line < line_starts_.size()
                                ? line_starts_[line]
                                : static_cast<std::uint32_t>(contents_.size());

  std::string_view text{contents_};
  text = text.substr(start, end - start);
  while (!text.empty() && (text.back() == '\n' || text.back() == '\r'))
    text.remove_suffix(1);
  return text;
}

} // namespace cherry::common
