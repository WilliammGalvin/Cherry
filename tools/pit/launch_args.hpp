#pragma once

#include <cassert>
#include <cstddef>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

enum class flag_type { lex_only, parse_only };

inline const std::unordered_map<std::string_view, flag_type> flag_types = {
    {"--lex-only", flag_type::lex_only},
    {"--parse-only", flag_type::parse_only},
};

class launch_args {
public:
  launch_args(int argc, char **argv) {
    assert(argc > 0);
    _launch_name = argv[0];

    const std::size_t n = static_cast<std::size_t>(argc);
    for (std::size_t i = 1; i < n; ++i) {
      const auto arg = std::string_view{argv[i]};
      if (arg.starts_with("-")) {
        const auto flag_opt = _parse_flag(arg);
        if (flag_opt)
          _flags.push_back(*flag_opt);
        else
          _errors.push_back(std::format("flag {} not valid.", arg));

        continue;
      }

      _args.emplace_back(arg);
    }
  }

  const std::string &launch_name() const { return _launch_name; }
  const std::vector<std::string> &args() const { return _args; }
  const std::vector<flag_type> &flags() const { return _flags; }
  const std::vector<std::string> &errors() const { return _errors; }

private:
  std::string _launch_name;
  std::vector<std::string> _args;
  std::vector<flag_type> _flags;
  std::vector<std::string> _errors;

  std::optional<flag_type> _parse_flag(std::string_view arg) const {
    const auto it = flag_types.find(arg);
    if (it == flag_types.end())
      return std::nullopt;

    return it->second;
  }
};
