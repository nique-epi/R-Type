#include "MissileStartPosition.hpp"
#include "CollisionConstants.hpp"
#include "EntitySizeConstants.hpp"
#include "Position.hpp"
#include "ShipControlConstants.hpp"

namespace rtype::game {

Position missileStartPosition(Position ship) {
  return Position{
      .x = ship.x + ((SHIP_WIDTH + PLAYER_MISSILE_WIDTH) * engine::HALF),
      .y = ship.y};
}

}  // namespace rtype::game
