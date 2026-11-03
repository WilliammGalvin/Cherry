#pragma once

#include <span>
#include <string_view>

namespace common::cli {

struct flag_spec {
  std::string_view long_name;
  char short_name{'\0'};
  bool takes_value{false};
  std::string_view value_name;
  std::string_view help;
};

struct command_spec {
  std::string_view name;
  std::string_view summary;
  std::string_view usage;
  std::span<const flag_spec> flags;
};

} // namespace common::cli
