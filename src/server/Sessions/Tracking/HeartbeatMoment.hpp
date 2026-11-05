#pragma once

#include <cstdint>
#include <optional>
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief When the Heartbeats of a tick are sent: the time on the clock of the
 * sessions, and the tick the game is at.
 */
struct HeartbeatMoment {
  /** @brief The time on the clock of the sessions: the steady clock the
   * network thread stamps each datagram with (see
   * IncomingDatagram::receivedAt). */
  engine::Duration now{};
  /** @brief The tick being simulated, told to the players in their Heartbeat
   * so their clock can follow the one of the game; empty while no game runs,
   * since the tick then stands still. */
  std::optional<std::uint32_t> tick = std::nullopt;
};

}  // namespace rtype::server
