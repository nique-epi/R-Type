#pragma once

#include "GameOutcome.hpp"
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
  /** @brief How the game ended, when current is Finished: Victory only when
   * its players cleared the level; a game the server ends by itself
   * (GameFlow::finish()) is a Defeat. Defeat in every other change. */
  GameOutcome outcome{GameOutcome::Defeat};

  bool operator==(const GameStateChanged&) const = default;
};

}  // namespace rtype::server
