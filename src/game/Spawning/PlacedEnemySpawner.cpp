#include "PlacedEnemySpawner.hpp"
#include <algorithm>
#include <cstddef>
#include <span>
#include <vector>
#include "CollisionConstants.hpp"
#include "IEntityOrders.hpp"
#include "LdtkPlacement.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"

namespace rtype::game {

namespace {

/** @returns The scroll at which @p placement comes in: its left edge on the
 * right edge of the playfield. */
float entryScrollOf(const LdtkPlacement& placement) {
  return placement.position.x - PLAYFIELD_WIDTH -
         (placement.width * engine::HALF);
}

}  // namespace

PlacedEnemySpawner::PlacedEnemySpawner(
    std::span<const LdtkPlacement> placements)
    : placements_(placements.begin(), placements.end()) {
  std::ranges::stable_sort(placements_, {}, entryScrollOf);
}

void PlacedEnemySpawner::restartAt(float scrollPosition) {
  next_ = static_cast<std::size_t>(
      std::ranges::upper_bound(placements_, scrollPosition, {}, entryScrollOf) -
      placements_.begin());
}

void PlacedEnemySpawner::spawnDue(float scrollPosition, IEntityOrders& orders) {
  while (next_ < placements_.size() &&
         entryScrollOf(placements_[next_]) <= scrollPosition) {
    const LdtkPlacement& placement = placements_[next_];
    static_cast<void>(orders.spawn(
        placement.sheet, Position{.x = placement.position.x - scrollPosition,
                                  .y = placement.position.y}));
    ++next_;
  }
}

}  // namespace rtype::game
