#pragma once

#include <cstdint>
#include <string>
#include "GameRuleConstants.hpp"
#include "ListedGameState.hpp"

namespace rtype::network {

/** @brief A game of a server, as the master lists it to a player. */
struct ListedGame {
  /** @brief Its number on the server, FIRST_HOSTED_GAME_NUMBER or more. */
  std::uint16_t number{};
  std::string name;
  ListedGameState state{ListedGameState::Waiting};
  /** @brief How many players are in it, MAXIMUM_PLAYERS at most. Never their
   * nicknames: those stay in the heartbeat. */
  std::uint8_t playerCount{};
  /** @brief The name of the difficulty it is played at. */
  std::string difficulty{identifiers::DEFAULT_DIFFICULTY};
  /** @brief The lives each of its players starts with. */
  std::uint8_t lives{identifiers::DEFAULT_LIVES};
  /** @brief Whether a missile of a player costs another player a life. */
  bool friendlyFire{false};

  bool operator==(const ListedGame&) const = default;
};

}  // namespace rtype::network
