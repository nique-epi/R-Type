#pragma once

#include <functional>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "PlayerInput.hpp"

namespace rtype::server {

/**
 * @brief What a game calls when something happens to one of its players: each
 * list holds the functions the modules added, and the game calls them in the
 * order they were added, that is the order of the modules.
 *
 * A module adds its functions in start(), once the parts they use exist. The
 * lists are not emptied by stop(): the game is destroyed right after its
 * modules stopped, and calls no hook in between. Not thread-safe: use it from
 * the simulation thread only.
 */
struct SessionHooks {
  /** @brief Called with a player who just took a place in the roster, for a
   * module to give it what it needs (a ship). */
  std::vector<std::function<void(ConnectedPlayer&)>> joined;
  /** @brief Called with the player whose session closes, after its seat is
   * freed and before its departure is announced, for a module to take what it
   * holds of the player out of the game. */
  std::vector<std::function<void(const ConnectedPlayer&)>> closing;
  /** @brief Called with each PlayerInput newer than the one kept for a player,
   * before it replaces that one, for a module to read the presses it holds. */
  std::vector<
      std::function<void(const ConnectedPlayer&, const network::PlayerInput&)>>
      newerInput;
};

}  // namespace rtype::server
