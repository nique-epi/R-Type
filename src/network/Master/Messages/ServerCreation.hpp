#pragma once

#include <string>

namespace rtype::network {

/** @brief What the web site sends to create a game server: its name. */
struct ServerCreation {
  /** @brief The name players read, MAXIMUM_SERVER_NAME_SIZE bytes at most. */
  std::string name;

  bool operator==(const ServerCreation&) const = default;
};

}  // namespace rtype::network
