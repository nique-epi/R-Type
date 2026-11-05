#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief Where a game stands for a player who reads the list of a server:
 * written `waiting`, `inGame` and `full` in JSON.
 */
enum class ListedGameState : std::uint8_t {
  /** @brief Waits for players and has a free seat. */
  Waiting,
  /** @brief Starting, running or just over. */
  InGame,
  /** @brief Waits for players and every seat is taken. */
  Full,
};

}  // namespace rtype::network
