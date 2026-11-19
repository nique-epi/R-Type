#pragma once

#include <string>

namespace rtype::network {

/**
 * @brief A map the players of a waiting room vote for, as LobbyState lists
 * it: the name the room shows, following the rules of a nickname
 * (isValidNickname()), and the path of its preview image in the content pack
 * of the server, a path isContentPath() accepts, or empty for a map without
 * one.
 */
struct LobbyMap {
  std::string name;
  std::string preview;

  bool operator==(const LobbyMap&) const = default;
};

}  // namespace rtype::network
