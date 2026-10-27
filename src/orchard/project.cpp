#include <orchard/project.hpp>

namespace orchard {

static constexpr bool is_valid_lib(const std::filesystem::path &lib_path) {
  return std::filesystem::exists(lib_path) &&
         std::filesystem::is_directory(lib_path);
}

project::project(const std::filesystem::path &root_path)
    : _root_path(root_path) {}

std::expected<void, project_error> project::load() {
  if (!is_valid_lib(_root_path))
    return std::unexpected(project_error::invalid_path);

  return {};
}

} // namespace orchard
