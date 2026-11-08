#pragma once

#include <cstdint>
#include <string>
#include "GameRules.hpp"
#include "ListedGameState.hpp"

namespace rtype::client {

/** @brief A game of a server as the panel of its games shows it. */
struct GameRow {
  /** @brief Its number on its server, from 1. */
  std::uint16_t number{};
  std::string name;
  network::ListedGameState state{network::ListedGameState::Waiting};
  std::uint8_t playerCount{};
  /** @brief The players a game of its server takes at most. */
  std::uint8_t maximumPlayers{};
  /** @brief The rules the game is played by, as the master lists them. */
  network::GameRules rules{network::defaultGameRules()};

  bool operator==(const GameRow&) const = default;
};

}  // namespace rtype::client
