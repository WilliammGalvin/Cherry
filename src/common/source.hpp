#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>
#include <vector>

namespace cherry::common {

// Owns a source buffer for the lifetime of compilation. Tokens and AST nodes
// hold string_views and byte offsets into it, so a source_file must outlive
// everything derived from it.
//
// Shared by both languages: Pit and Cherry read files the same way.
class source_file {
public:
  static std::expected<source_file, std::string> load(std::string_view path);

  // For tests and REPL input, where there is no file on disk.
  static source_file from_string(std::string path, std::string contents);

  [[nodiscard]] std::string_view path() const noexcept { return path_; }
  [[nodiscard]] std::string_view text() const noexcept { return contents_; }

  struct position {
    std::uint32_t line = 1;   // 1-based
    std::uint32_t column = 1; // 1-based
  };

  // Maps a byte offset to a human-readable position. O(log n).
  [[nodiscard]] position position_of(std::uint32_t offset) const noexcept;

  // The full text of a 1-based line, without its terminator. Used to render
  // the source line above a caret in a diagnostic.
  [[nodiscard]] std::string_view line_text(std::uint32_t line) const noexcept;

  [[nodiscard]] std::uint32_t line_count() const noexcept {
    return static_cast<std::uint32_t>(line_starts_.size());
  }

private:
  source_file(std::string path, std::string contents);

  std::string path_;
  std::string contents_;
  std::vector<std::uint32_t>
      line_starts_; // offset of the first byte of each line
};

} // namespace cherry::common
