#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief Where a game server stands for a player or an admin, as the master
 * computes it from what the server sends: written `online`, `degraded`,
 * `offline`, `maintenance` and `unreachable` in JSON.
 */
enum class ServerStatus : std::uint8_t {
  /** @brief Beats on time, its ticks short enough and its losses low enough. */
  Online,
  /** @brief Beats on time, but its ticks are too long or it loses too much. */
  Degraded,
  /** @brief Has sent nothing for three heartbeats. */
  Offline,
  /** @brief Taken out of service by an admin, whatever it sends. */
  Maintenance,
  /** @brief Beats on time, but no probe of the master got a valid answer for
   * PROBE_ANSWER_SILENCE, or since it registered: its UDP port cannot be
   * reached. */
  Unreachable,
};

}  // namespace rtype::network
