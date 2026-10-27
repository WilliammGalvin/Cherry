#pragma once

#include <expected>
#include <filesystem>
#include <orchard/manifest/manifest.hpp>

namespace orchard {

enum class project_error {
  invalid_path,
};

class project {
public:
  explicit project(const std::filesystem::path &root_path);

  std::expected<void, project_error> load();

private:
  std::filesystem::path _root_path;
  manifest _root_manifest;
};

} // namespace orchard
