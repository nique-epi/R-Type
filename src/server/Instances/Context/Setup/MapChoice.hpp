#pragma once

#include <string>

namespace rtype::server {

/**
 * @brief A map the players of a waiting room may vote for: the level a script
 * registered under `level`, the name the room shows, which the script gave
 * with the option map of `level.register`, and the path of its preview in the
 * content pack, from the option preview (game::mapPreviewPathOf()), empty for
 * a map without one.
 */
struct MapChoice {
  std::string level;
  std::string name;
  std::string preview;

  bool operator==(const MapChoice&) const = default;
};

}  // namespace rtype::server
