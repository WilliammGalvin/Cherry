#pragma once

#include <orchard/manifest/lexer.hpp>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace orchard::manifest {

enum class parse_error {
  //
};

using manifest_value =
    std::variant<int, bool, std::string, std::vector<std::string>>;
using manifest_entry = std::pair<std::string, manifest_value>;
using manifest_data = std::vector<manifest_entry>;

class parser {
public:
  using token_vector = const lexer::token_vector &;
  using parse_result = std::expected<manifest_data, parse_error>;

  explicit parser(token_vector tokens) noexcept : _tokens(tokens) {}

  [[nodiscard]] parse_result parse();

private:
  token_vector _tokens;
};

} // namespace orchard::manifest
