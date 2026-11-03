#pragma once

#include <cstdint>

namespace pit::driver {

enum class compile_status : std::uint8_t {
  ok,
  user_errors,
  io_failure,
};

struct compile_result {
  compile_status status{compile_status::ok};
  std::uint32_t error_count{0};

  constexpr bool ok() const { return status == compile_status::ok; }

  constexpr int exit_code() const {
    switch (status) {
    case compile_status::ok:
      return 0;
    case compile_status::user_errors:
      return 1;
    case compile_status::io_failure:
      return 2;
    }

    return 1;
  }
};

} // namespace pit::driver
