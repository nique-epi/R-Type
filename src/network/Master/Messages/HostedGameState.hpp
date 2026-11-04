#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief Where a game a server hosts stands, as its heartbeat tells the
 * master: written `waiting`, `starting`, `playing` and `finished` in JSON.
 */
enum class HostedGameState : std::uint8_t {
  Waiting,
  Starting,
  Playing,
  Finished,
};

}  // namespace rtype::network
