#pragma once

#include <common/cli/spec.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace common::cli {

struct parsed_flag {
  const flag_spec *spec;
  std::optional<std::string_view> value;
};

struct parsed {
  std::vector<parsed_flag> flags;
  std::vector<std::string_view> positionals;
  std::vector<std::string_view> passthrough;

  bool has(std::string_view long_name) const;
  std::optional<std::string_view> value(std::string_view long_name) const;
};

struct help_requested {};

struct usage_error {
  std::string message;
};

using parse_result = std::variant<parsed, help_requested, usage_error>;

} // namespace common::cli
