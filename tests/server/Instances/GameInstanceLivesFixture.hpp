#pragma once

#include <cstddef>
#include <cstdint>
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

  /**
   * @brief Puts a missile of an enemy on the ship of @p player, then runs the
   * tick that finds them touching, the bus delivering its events.
   * @returns The network id of the missile.
   */
  std::uint32_t shootTheShipOf(std::size_t player);

  /** @brief Both players say ready again, then the tick that starts the next
   * game runs. */
  void startTheNextGame();

  /** @returns The player at @p index; a default one, after a failure, when it
   * is not in the roster. */
  [[nodiscard]] ConnectedPlayer playerAt(std::size_t index);

  /** @returns How the newest WorldState sent to the player at @p index lists
   * that player; a default one, after a failure, when none does. */
  [[nodiscard]] network::PlayerState newestStateOf(std::size_t index);

  /** @brief Leaves @p player one life, then puts a Drifter on its ship and runs
   * the tick that finds them touching, the bus delivering its events: the
   * player has no life left. */
  void takeTheLastLifeOf(std::size_t player);

  /** @brief Takes the last life of both players, then runs the tick that finds
   * no life left in the game, which ends it. */
  void loseEveryLife();

  /** @brief Gives the first player FIRST_PLAYER_FINAL_SCORE points and the
   * second SECOND_PLAYER_FINAL_SCORE. */
  void giveTheFinalScores();
};

}  // namespace rtype::server::testing
