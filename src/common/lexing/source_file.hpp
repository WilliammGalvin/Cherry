#include <expected>
#include <filesystem>
#include <string>

namespace common::lexing {

enum class source_file_error {
  not_found,
  unreadable,
  too_large,
};

class source_file {
public:
  explicit source_file(std::filesystem::path path) : _path(path) {}

  std::expected<void, source_file_error> load();

private:
  std::filesystem::path _path;
  std::string _content{};
  bool _loaded{false};
};

} // namespace common::lexing
