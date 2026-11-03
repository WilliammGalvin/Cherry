#include "common/io/source_manager.hpp"
#include <algorithm>
#include <expected>
#include <filesystem>
#include <orchard/config.hpp>
#include <orchard/manifest/manifest.hpp>
#include <orchard/project/project.hpp>
#include <system_error>

namespace orchard {

std::expected<void, project_error> project::scan() {
  const auto root_validate_res = _validate_project_root();
  if (!root_validate_res)
    return std::unexpected(root_validate_res.error());

  return {};
}

std::expected<void, project_error> project::_validate_project_root() const {
  if (!std::filesystem::exists(_root) || !std::filesystem::is_directory(_root))
    return std::unexpected(project_error::invalid_root_path);

  const auto build_manifest_res = _find_build_manifest();
  if (!build_manifest_res)
    return std::unexpected(build_manifest_res.error());

  const auto build_id = _source_manager.add_source(build_manifest_res.value());
  if (!build_id)
    return std::unexpected(project_error::source_manager_error);

  manifest::manifest_data data =
      manifest::from_source(build_id.value(), _source_manager);

  //

  return {};
}

std::expected<project::file_path, project_error>
project::_find_build_manifest() const {
  std::error_code ec;
  std::filesystem::directory_iterator it(_root, ec);
  if (ec)
    return std::unexpected(project_error::invalid_root_path);

  for (const auto &entry : it) {
    if (!entry.is_regular_file())
      continue;

    const auto &path = entry.path();
    if (path.stem() != config::project_manifest_filename)
      continue;

    if (std::ranges::contains(config::project_manifest_extensions,
                              path.extension()))
      return path;
  }

  return std::unexpected(project_error::manifest_not_found);
}

} // namespace orchard
