#include "ShipGunReadingFixtures.hpp"
#include "ShipGun.hpp"
#include "ShipGunReadingTestConstants.hpp"

namespace rtype::game::testing {

ShipGun shippedGun() {
  return ShipGun{.shipType = SHIPPED_SHIP_NUMBER,
                 .projectileType = SHIPPED_PLAYER_MISSILE_NUMBER,
                 .intervalTicks = SHIPPED_GUN_INTERVAL_TICKS,
                 .firstShotTicks = SHIPPED_GUN_FIRST_SHOT_TICKS,
                 .velocityX = SHIPPED_PLAYER_MISSILE_SPEED,
                 .velocityY = 0.0F};
}

}  // namespace rtype::game::testing
