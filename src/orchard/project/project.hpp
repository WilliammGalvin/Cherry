#pragma once

#include <common/io/source_manager.hpp>
#include <expected>
#include <filesystem>
#include <orchard/project/module.hpp>
#include <vector>

namespace orchard {

enum class project_error {
  invalid_root_path,
  manifest_not_found,
  source_manager_error,
  src_directory_not_found,
};

class project {
public:
  using file_path = std::filesystem::path;

  explicit project(file_path root) : _root(std::move(root)) {}

  [[nodiscard]] std::expected<void, project_error> scan();

  const std::vector<module> &modules() const { return _modules; }

  bool scanned() const { return _scanned; }

private:
  file_path _root;
  common::io::source_manager _source_manager;
  std::vector<module> _modules;
  bool _scanned{false};

  [[nodiscard]] std::expected<void, project_error>
  _validate_project_root() const;

  [[nodiscard]] std::expected<file_path, project_error>
  _find_build_manifest() const;
};

} // namespace orchard
