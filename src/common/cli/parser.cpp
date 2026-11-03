#include "common/cli/parse_result.hpp"
#include <algorithm>
#include <common/cli/parser.hpp>
#include <cstddef>
#include <optional>
#include <span>
#include <string_view>

namespace common::cli {

bool parsed::has(std::string_view long_name) const {
  return std::ranges::any_of(flags, [&](const parsed_flag &flag) {
    return flag.spec->long_name == long_name;
  });
}

std::optional<std::string_view>
parsed::value(std::string_view long_name) const {
  for (auto it = flags.rbegin(); it != flags.rend(); ++it) {
    if (it->spec->long_name == long_name)
      return it->value;
  }

  return std::nullopt;
}

namespace {

const flag_spec *find_long(const command_spec &spec, std::string_view name) {
  for (const flag_spec &flag : spec.flags) {
    if (!flag.long_name.empty() && flag.long_name == name)
      return &flag;
  }

  return nullptr;
}

const flag_spec *find_short(const command_spec &spec, char name) {
  for (const flag_spec &flag : spec.flags) {
    if (flag.short_name == name)
      return &flag;
  }

  return nullptr;
}

parse_result make_usage_error(std::string message) {
  return usage_error{std::move(message)};
}

} // namespace

parse_result parse(const command_spec &spec,
                   std::span<const std::string_view> args) {
  parsed out;
  for (std::size_t i = 0; i < args.size(); ++i) {
    const std::string_view arg = args[i];
    if (arg == "--") {
      out.passthrough.assign(args.begin() + static_cast<std::ptrdiff_t>(i) + 1,
                             args.end());
      break;
    }

    if (arg.starts_with("--")) {
      std::string_view name = arg.substr(2);
      std::optional<std::string_view> inline_value;
      if (const auto eq = name.find('='); eq != std::string_view::npos) {
        inline_value = name.substr(eq + 1);
        name = name.substr(0, eq);
      }

      const flag_spec *flag = find_long(spec, name);
      if (!flag) {
        if (name == "help" && !inline_value)
          return help_requested{};

        return make_usage_error("unknown flag '--'" + std::string(name) + "'");
      }

      if (flag->takes_value) {
        if (inline_value) {
          out.flags.push_back({flag, inline_value});
        } else if (i + 1 < args.size()) {
          out.flags.push_back({flag, args[++i]});
        } else {
          return make_usage_error("missing value for flag '--'" +
                                  std::string(name) + "'");
        }
      } else {
        if (inline_value) {
          return make_usage_error("flag '--'" + std::string(name) +
                                  "' does not take a value");
        }

        out.flags.push_back({flag, std::nullopt});
      }

      continue;
    }

    if (arg.size() > 1 && arg.front() == '-') {
      for (std::size_t j = 1; j < args.size(); ++j) {
        const char c = arg[j];
      }
    }
  }
}

} // namespace common::cli
