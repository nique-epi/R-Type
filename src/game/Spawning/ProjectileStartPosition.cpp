#include "ProjectileStartPosition.hpp"
#include "CollisionBox.hpp"
#include "CollisionConstants.hpp"
#include "EntityType.hpp"
#include "Muzzle.hpp"
#include "Position.hpp"
#include "SpawningConstants.hpp"

namespace rtype::game {

Position projectileStartPosition(const Muzzle& muzzle,
                                 CollisionBox projectile) {
  const float facing =
      muzzle.side == EntityType::Player ? FACING_RIGHT : FACING_LEFT;
  return Position{
      .x = muzzle.center.x +
           (facing * (muzzle.box.width + projectile.width) * engine::HALF),
      .y = muzzle.center.y};
}

}  // namespace rtype::game
