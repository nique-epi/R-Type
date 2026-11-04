#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief Where a game a server hosts stands, as its heartbeat tells the
 * master: written `waiting`, `starting`, `playing`, `finished` and `full` in
 * JSON.
 */
enum class HostedGameState : std::uint8_t {
  Waiting,
  Starting,
  Playing,
  Finished,
  /** @brief Waits for players, and every seat is taken. */
  Full,
};

}  // namespace rtype::network
