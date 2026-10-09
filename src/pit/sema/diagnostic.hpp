#pragma once

#include "file_id.hpp"
#include "pit/ast/node.hpp"
#include "pit/sema/arena.hpp"

#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

namespace pit::sema {

enum class severity : std::uint8_t {
  error,
  warning,
  note,
};

struct diagnostic {
  severity sev{severity::error};
  file_id file{file_id::none};
  ast::token_id token{ast::token_id::none};
  std::string_view message;
};

class diagnostic_sink {
public:
  explicit diagnostic_sink(arena &messages) : _arena(messages) {}

  diagnostic_sink(const diagnostic_sink &) = delete;
  diagnostic_sink &operator=(const diagnostic_sink &) = delete;

  void report(severity sev, file_id file, ast::token_id token,
              std::string_view message) {
    std::string_view owned;
    if (!message.empty()) {
      auto copy = _arena.allocate_copy<char>(std::span<const char>(message));
      owned = {copy.data(), copy.size()};
    }

    _diags.push_back({sev, file, token, owned});
    if (sev == severity::error)
      ++_errors;
    else if (sev == severity::warning)
      ++_warnings;
  }

  template <typename... Args>
  void error(file_id file, ast::token_id token, std::format_string<Args...> fmt,
             Args &&...args) {
    report(severity::error, file, token,
           std::format(fmt, std::forward<Args>(args)...));
  }

  template <typename... Args>
  void warning(file_id file, ast::token_id token,
               std::format_string<Args...> fmt, Args &&...args) {
    report(severity::warning, file, token,
           std::format(fmt, std::forward<Args>(args)...));
  }

  template <typename... Args>
  void note(file_id file, ast::token_id token, std::format_string<Args...> fmt,
            Args &&...args) {
    report(severity::note, file, token,
           std::format(fmt, std::forward<Args>(args)...));
  }

  bool has_errors() const { return _errors != 0; }
  bool error_count() const { return _errors; }
  bool warning_count() const { return _warnings; }

  std::span<const diagnostic> all() const { return _diags; }

private:
  arena &_arena;
  std::vector<diagnostic> _diags;
  std::size_t _errors{0};
  std::size_t _warnings{0};
};

} // namespace pit::sema
