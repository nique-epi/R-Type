#pragma once

#include <string_view>
#include "NetworkConditions.hpp"

namespace rtype::network {

/** @brief A simulated network known by name, given at launch with
 * `--network <name>`. */
struct NetworkScenario {
  std::string_view name;
  NetworkConditions conditions;
};

}  // namespace rtype::network
