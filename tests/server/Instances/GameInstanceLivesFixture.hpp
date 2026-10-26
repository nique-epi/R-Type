#pragma once

#include <cstddef>
#include "ConnectedPlayer.hpp"
#include "GameInstanceShootingFixture.hpp"
#include "WorldState.hpp"

namespace rtype::server::testing {

/**
 * @brief The shooting fixture, whose event bus delivers the events of a tick
 * at its end, with ships that can be put on a Bydo and the lives of the
 * players as the game and the world state hold them.
 */
class GameInstanceLivesFixture : public GameInstanceShootingFixture {
 protected:
  /** @brief Puts a Drifter on the ship of @p player, then runs the tick that
   * finds them touching, the bus delivering its events. */
  void hitTheShipOf(std::size_t player);

  /** @brief Both players say ready again, then the tick that starts the next
   * game runs. */
  void startTheNextGame();

  /** @returns The player at @p index; a default one, after a failure, when it
   * is not in the roster. */
  [[nodiscard]] ConnectedPlayer playerAt(std::size_t index);

  /** @returns How the newest WorldState sent to the player at @p index lists
   * that player; a default one, after a failure, when none does. */
  [[nodiscard]] network::PlayerState newestStateOf(std::size_t index);
};

}  // namespace rtype::server::testing
