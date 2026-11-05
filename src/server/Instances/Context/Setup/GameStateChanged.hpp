#pragma once

#include "GameState.hpp"

namespace rtype::server {

/**
 * @brief Published on the bus of a game each time GameFlow moves it from one
 * state to another, so a reader knows the transition without keeping the
 * state it saw before.
 */
struct GameStateChanged {
  GameState previous{GameState::WaitingForPlayers};
  GameState current{GameState::WaitingForPlayers};

  bool operator==(const GameStateChanged&) const = default;
};

}  // namespace rtype::server
