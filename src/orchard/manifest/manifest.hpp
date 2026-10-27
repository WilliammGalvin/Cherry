#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace orchard {

using manifest_value =
    std::variant<int, bool, std::string, std::vector<std::string>>;

class manifest {
public:
private:
  std::unordered_map<std::string, manifest_value> _pairs;
};

} // namespace orchard
