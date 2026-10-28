#pragma once

#include <cstdint>

namespace rtype::server {

/**
 * @brief Where a game is in its life, see GameFlow.
 */
enum class GameState : std::uint8_t {
  WaitingForPlayers,
  InProgress,
  Finished,
};

}  // namespace rtype::server
