#pragma once

#include <array>
#include <string_view>

namespace orchard::config {

using str = std::string_view;

inline constexpr std::string_view project_metadata_filename = "build";

inline constexpr auto project_metadata_extensions =
    std::to_array<str>({".orch", ".orchard"});

inline constexpr auto pit_extensions = std::to_array<str>({".pit"});

inline constexpr auto cherry_extensions =
    std::to_array<str>({".ch", ".cherry"});

} // namespace orchard::config
