#pragma once

#include <common/cli/parse_result.hpp>
#include <common/cli/spec.hpp>
#include <span>
#include <string_view>

namespace common::cli {

parse_result parse(const command_spec &spec,
                   std::span<const std::string_view> args);

}
