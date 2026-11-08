#pragma once

#include "GameState.hpp"

namespace rtype::server {

/**
 * @brief What the placement of a new player needs to know of a game: its state
 * and whether its seats are all taken. It describes a game the caller may not
 * own, so the rule of firstJoinableGame() reads no GameInstance.
 */
struct GameVacancy {
  GameState state{GameState::WaitingForPlayers};
  bool full{false};
  /** @brief Whether a player who takes any game may be sent to it: false for
   * a game whose creator chose other rules than the default ones. */
  bool takesAnyPlayer{true};
};

}  // namespace rtype::server
