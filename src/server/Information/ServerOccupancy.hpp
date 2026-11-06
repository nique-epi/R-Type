#pragma once

#include <cstdint>

namespace rtype::server {

/** @brief How full the server is, as a ServerInformation says it. */
struct ServerOccupancy {
  /** @brief The players seated in its games, away ones included. */
  std::uint16_t playerCount{};
  /** @brief The games it may run at once, whatever runs now. */
  std::uint16_t gameCount{};
  /** @brief The seats a new player may take: the empty seats of the games
   * that wait for players, and those of the games it may still start. */
  std::uint16_t freeSeatCount{};
};

}  // namespace rtype::server
