#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "HostedGameState.hpp"

namespace rtype::network {

/** @brief A game a server hosts, as its heartbeat lists it to the master. */
struct HostedGame {
  /** @brief Its number on the server, FIRST_HOSTED_GAME_NUMBER or more. */
  std::uint16_t number{};
  HostedGameState state{HostedGameState::Waiting};
  /** @brief The name of the difficulty it is played in. */
  std::string difficulty;
  /** @brief The nickname of each player in it, MAXIMUM_PLAYERS at most. */
  std::vector<std::string> players;

  bool operator==(const HostedGame&) const = default;
};

}  // namespace rtype::network
