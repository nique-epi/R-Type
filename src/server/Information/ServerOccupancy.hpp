#pragma once

#include <cstdint>

namespace rtype::server {

/** @brief How full the server is, as a ServerInformation says it. */
struct ServerOccupancy {
  /** @brief The players seated in its games, away ones included. */
  std::uint16_t playerCount{};
  /** @brief The games it hosts, whatever their state. */
  std::uint16_t gameCount{};
  /** @brief The empty seats of the games that wait for players. */
  std::uint16_t freeSeatCount{};
};

}  // namespace rtype::server
