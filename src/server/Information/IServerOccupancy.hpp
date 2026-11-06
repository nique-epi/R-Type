#pragma once

#include "ServerOccupancy.hpp"

namespace rtype::server {

/**
 * @brief How full the server is, for the information module, which tells it
 * to whoever asks: the supervised games module counts it from its games. Read
 * from the simulation thread only.
 */
class IServerOccupancy {
 public:
  virtual ~IServerOccupancy() = default;

  /** @returns The players, the games and the free seats of the server now. */
  [[nodiscard]] virtual ServerOccupancy occupancy() const = 0;
};

}  // namespace rtype::server
