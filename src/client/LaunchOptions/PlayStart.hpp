#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Where PLAY leads from the title. */
enum class PlayStart : std::uint8_t {
  /** @brief The server map, which lists the servers of the master. */
  ServerMap,
  /** @brief A server joined at once: the server of the last attempt, the one
   * given at launch with `-h` before any. */
  GivenServer,
};

}  // namespace rtype::client
