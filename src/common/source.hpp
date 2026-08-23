#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>
#include <vector>

namespace cherry::common {

class source_file {
public:
  static std::expected<source_file, std::string> load(std::string_view path);

  static source_file from_string(std::string path, std::string contents);

  std::string_view path() const noexcept { return path_; }
  std::string_view text() const noexcept { return contents_; }

  struct position {
    std::uint32_t line{1};
    std::uint32_t column{1};
  };

  position position_of(std::uint32_t offset) const noexcept;

  std::string_view line_text(std::uint32_t line) const noexcept;

  std::uint32_t line_count() const noexcept {
    return static_cast<std::uint32_t>(line_starts_.size());
  }

private:
  source_file(std::string path, std::string contents);

  std::string path_;
  std::string contents_;
  std::vector<std::uint32_t> line_starts_{};
};

} // namespace cherry::common
