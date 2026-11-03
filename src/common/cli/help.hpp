#pragma once

#include <algorithm>
#include <common/cli/spec.hpp>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace common::cli {

namespace {

struct row {
  std::string left;
  std::string_view help;
};

row make_row(const flag_spec &flag) {
  std::string left;
  if (flag.short_name != '\0') {
    left += '-';
    left += flag.short_name;

    if (!flag.long_name.empty())
      left += ", ";
  } else {
    left += "    ";
  }

  if (!flag.long_name.empty()) {
    left += "--";
    left += flag.long_name;
  }

  if (flag.takes_value) {
    left += " <";
    left +=
        flag.value_name.empty() ? std::string_view("value") : flag.value_name;
    left += '>';
  }

  return {std::move(left), flag.help};
}

} // namespace

std::string render_help(std::string_view program, const command_spec &spec) {
  std::string out = "Usage: ";
  out += program;

  if (!spec.name.empty()) {
    out += ' ';
    out += spec.name;
  }

  out += " [flags]";
  if (!spec.usage.empty()) {
    out += ' ';
    out += spec.usage;
  }
  out += "\n";

  if (!spec.summary.empty()) {
    out += "\n";
    out += spec.summary;
    out += "\n";
  }

  std::vector<row> rows;
  rows.reserve(spec.flags.size() + 1);
  for (const flag_spec &flag : spec.flags)
    rows.push_back(make_row(flag));

  const bool defines_help =
      std::ranges::any_of(spec.flags, [](const flag_spec &flag) {
        return flag.long_name == "help" || flag.short_name == 'h';
      });

  if (!defines_help)
    rows.push_back({"  -h, --help", "Show this help"});

  std::size_t width = 0;
  for (const row &r : rows)
    width = std::max(width, r.left.size());

  out += "\nFlags:\n";
  for (const row &r : rows) {
    out += r.left;
    out.append(width - r.left.size() + 2, ' ');
    out += r.help;
    out += "\n";
  }

  return out;
}

} // namespace common::cli
