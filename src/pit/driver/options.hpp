#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <vector>

namespace pit::driver {

enum class emit_kind : std::uint8_t { exe, obj, ir };

enum class stage : std::uint8_t { lex, parse, sema, codegen };

enum class optimization_level : std::uint8_t { o0, o1, o2, o3 };

struct compile_options {
  using file_path = std::filesystem::path;

  std::vector<file_path> input_paths;
  std::optional<file_path> output_path;

  emit_kind emit{emit_kind::exe};
  optimization_level opt_level{optimization_level::o0};

  std::optional<stage> stop_after;

  bool dump_tokens{false};
  bool dump_ast{false};
  bool dump_ir{false};

  std::uint32_t error_limit = 20;
};

} // namespace pit::driver
