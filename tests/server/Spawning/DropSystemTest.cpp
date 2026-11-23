#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "DropSystemFixture.hpp"
#include "DropSystemTestConstants.hpp"
#include "EntitySpawned.hpp"

namespace rtype::server::testing {

/**
 * Given a red crystal dropped by an enemy, delivered by the bus
 * When no tick ran yet
 * Then the world holds no crystal: it appears out of the delivery
 */
TEST_F(DropSystemFixture, WaitsForItsTickToDropACrystal) {
  connect(FIRST_DROP_PLAYER);

  dropRedAt(WHERE_THE_CARRIER_DIED);

  EXPECT_EQ(crystalsInTheWorld(), 0U);
}

/**
 * Given a red crystal dropped by an enemy, delivered by the bus
 * When a tick runs
 * Then the world holds one crystal
 */
TEST_F(DropSystemFixture, MakesTheDroppedCrystalAppear) {
  connect(FIRST_DROP_PLAYER);
  dropRedAt(WHERE_THE_CARRIER_DIED);

  tick();

  EXPECT_EQ(crystalsInTheWorld(), 1U);
}

/**
 * Given two players and a red crystal dropped where an enemy died
 * When a tick runs
 * Then each player is sent the EntitySpawned of the red crystal, where the
 * enemy died
 */
TEST_F(DropSystemFixture, TellsEveryPlayerWhereTheCrystalAppears) {
  connect(FIRST_DROP_PLAYER);
  connect(SECOND_DROP_PLAYER);
  dropRedAt(WHERE_THE_CARRIER_DIED);

  tick();

  for (const std::size_t player : {FIRST_DROP_PLAYER, SECOND_DROP_PLAYER}) {
    const std::vector<network::EntitySpawned> spawns = spawnsSentTo(player);
    ASSERT_EQ(spawns.size(), 1U);
    EXPECT_EQ(spawns.front().entityType, redCrystalNumber());
    EXPECT_FLOAT_EQ(spawns.front().x, WHERE_THE_CARRIER_DIED.x);
    EXPECT_FLOAT_EQ(spawns.front().y, WHERE_THE_CARRIER_DIED.y);
  }
}

/**
 * Given a red crystal that already appeared at a tick
 * When another tick runs
 * Then no second crystal appears: each drop makes one crystal
 */
TEST_F(DropSystemFixture, DropsEachCrystalOnce) {
  connect(FIRST_DROP_PLAYER);
  dropRedAt(WHERE_THE_CARRIER_DIED);
  tick();

  tick();

  EXPECT_EQ(crystalsInTheWorld(), 1U);
}

}  // namespace rtype::server::testing
