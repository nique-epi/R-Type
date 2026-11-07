#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Where PLAY leads from the title. */
enum class PlayStart : std::uint8_t {
  /** @brief The server map, which lists the servers of the master. */
  ServerMap,
  /** @brief The server given at launch with `-h`, joined at once. */
  GivenServer,
};

}  // namespace rtype::client
