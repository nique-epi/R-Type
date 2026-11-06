#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief How the read of a list stands: of the servers, or of the games of a
 * server. */
enum class ServerListPhase : std::uint8_t {
  /** @brief Asking the master, or measuring the pings; nothing to show yet. */
  Searching,
  /** @brief The list holds at least one row. */
  Ready,
  /** @brief The master answered, and nothing is to be shown: no server online
   * and reachable, or no game on the server. */
  Empty,
  /** @brief The master did not answer, refused, or answered what cannot be
   * read. */
  MasterUnreachable,
};

}  // namespace rtype::client
