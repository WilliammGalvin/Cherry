#pragma once

namespace orchard {

enum class source_file_type {
  unknown,
  pit,
  orchard,
};

struct source_file {
  source_file_type type{source_file_type::unknown};
};

} // namespace orchard
