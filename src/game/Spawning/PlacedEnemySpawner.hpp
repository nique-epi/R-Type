#pragma once

#include <cstddef>
#include <span>
#include <vector>
#include "IEntityOrders.hpp"
#include "LdtkPlacement.hpp"

namespace rtype::game {

/**
 * @brief Makes the enemies a level places appear as the level scrolls: each
 * one once its left edge reaches the right edge of the playfield, at its
 * place, through the orders of the game.
 *
 * The placements are taken in the order they come in, the ones already past
 * when the level starts (on its first screen) at the first spawnDue(). A
 * restart from a checkpoint calls restartAt() with the scroll of the
 * checkpoint: the enemies that came in at or before it stay gone, the next
 * ones come in again. More than MAXIMUM_SPAWN_ORDERS_PER_TICK placements due
 * in one tick make the orders throw, which ends the game: a level places
 * fewer in one step of the scroll.
 */
class PlacedEnemySpawner {
 public:
  /** @param placements The enemies of the level; copied, in any order. */
  explicit PlacedEnemySpawner(std::span<const LdtkPlacement> placements);

  /** @brief Makes the next spawnDue() start from the first placement that
   * comes in after @p scrollPosition. */
  void restartAt(float scrollPosition);

  /** @brief Asks @p orders for every placement not yet spawned that comes in
   * at @p scrollPosition or before, each at its place on the screen.
   * @throws What IEntityOrders::spawn() throws. */
  void spawnDue(float scrollPosition, IEntityOrders& orders);

 private:
  std::vector<LdtkPlacement> placements_;
  std::size_t next_{0};
};

}  // namespace rtype::game
