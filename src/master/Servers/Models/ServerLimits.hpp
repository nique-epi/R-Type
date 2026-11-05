#pragma once

#include <cstdint>

namespace rtype::master {

/** @brief The bounds a registered server set, which its heartbeats keep. */
struct ServerLimits {
  std::uint16_t maximumGames{};
  std::uint8_t maximumPlayersPerGame{};
};

}  // namespace rtype::master
