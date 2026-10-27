#pragma once

#include <string>

namespace common::lexing {

struct source_file {
  std::string path;
  std::string text;
};

} // namespace common::lexing
