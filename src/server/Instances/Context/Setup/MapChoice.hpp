#pragma once

#include <string>

namespace rtype::server {

/**
 * @brief A map the players of a waiting room may vote for: the level a script
 * registered under `level`, and the name the room shows, which the script
 * gave with the option map of `level.register`.
 */
struct MapChoice {
  std::string level;
  std::string name;

  bool operator==(const MapChoice&) const = default;
};

}  // namespace rtype::server
