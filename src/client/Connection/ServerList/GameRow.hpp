#pragma once

#include <cstdint>
#include <string>
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

  bool operator==(const GameRow&) const = default;
};

}  // namespace rtype::client
