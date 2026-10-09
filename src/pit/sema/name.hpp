#pragma once

#include <cstdint>

namespace pit::sema {

enum class name_id : std::uint32_t { none = 0 };

constexpr bool is_valid(name_id n) { return n != name_id::none; }

}; // namespace pit::sema
