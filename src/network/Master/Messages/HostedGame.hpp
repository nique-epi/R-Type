#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "GameRuleConstants.hpp"
#include "HostedGameState.hpp"

namespace rtype::network {

/** @brief A game a server hosts, as its heartbeat lists it to the master. */
struct HostedGame {
  /** @brief Its number on the server, FIRST_HOSTED_GAME_NUMBER or more. */
  std::uint16_t number{};
  /** @brief The name players read, MAXIMUM_GAME_NAME_SIZE bytes at most. */
  std::string name;
  HostedGameState state{HostedGameState::Waiting};
  /** @brief The name of the difficulty it is played in. */
  std::string difficulty;
  /** @brief The lives each of its players starts with. */
  std::uint8_t lives{identifiers::DEFAULT_LIVES};
  /** @brief Whether a missile of a player costs another player a life. */
  bool friendlyFire{false};
  /** @brief The nickname of each player in it, MAXIMUM_PLAYERS at most. */
  std::vector<std::string> players;

  bool operator==(const HostedGame&) const = default;
};

}  // namespace rtype::network
