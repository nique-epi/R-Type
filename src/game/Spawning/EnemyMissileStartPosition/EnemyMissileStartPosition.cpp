#include "EnemyMissileStartPosition.hpp"
#include "CollisionConstants.hpp"
#include "EntitySizeConstants.hpp"
#include "Position.hpp"

namespace rtype::game {

Position enemyMissileStartPosition(Position bydo) {
  return Position{
      .x = bydo.x - ((BYDO_WIDTH + ENEMY_MISSILE_WIDTH) * engine::HALF),
      .y = bydo.y};
}

}  // namespace rtype::game
