#include <gtest/gtest.h>
#include <vector>
#include "Entity.hpp"
#include "LagCompensatedHitSystemShotDownFixture.hpp"
#include "LagCompensatedHitSystemTestConstants.hpp"
#include "MissileHit.hpp"

using rtype::engine::Entity;
using rtype::game::MissileHit;
using rtype::game::testing::FIVE_TICKS_BEHIND;
using rtype::game::testing::LagCompensatedHitSystemShotDownFixture;
using rtype::game::testing::WHERE_THE_PLAYER_SAW_THE_ENEMY;

/**
 * Given a Shootable missile of an enemy that stood at a place for one tick,
 * then went 200 units higher for four ticks
 * When a missile of a player reaches that place, its player five ticks behind
 * Then it hits that missile: its player saw the missile there
 */
TEST_F(LagCompensatedHitSystemShotDownFixture,
       AShotHitsAMissileWhereItsPlayerSawIt) {
  const Entity missile = shootableMissileThatLeft();
  const Entity shot = shotAt(WHERE_THE_PLAYER_SAW_THE_ENEMY, FIVE_TICKS_BEHIND);

  runTicks(1);

  EXPECT_EQ(hits(), (std::vector<MissileHit>{
                        MissileHit{.missile = shot, .target = missile}}));
}

/**
 * Given the same Shootable missile, gone 200 units higher for four ticks
 * When a missile of a player reaches the place it left, its player seeing the
 * present
 * Then it hits nothing: the missile is no longer there in the present
 */
TEST_F(LagCompensatedHitSystemShotDownFixture,
       AShotWithoutDelayMissesAMissileThatLeft) {
  static_cast<void>(shootableMissileThatLeft());
  static_cast<void>(shotAt(WHERE_THE_PLAYER_SAW_THE_ENEMY, 0));

  runTicks(1);

  EXPECT_TRUE(hits().empty());
}

/**
 * Given a missile of an enemy that is not Shootable
 * When a missile of a player whose player sees the present lies on it
 * Then nothing is hit: the two fly through each other
 */
TEST_F(LagCompensatedHitSystemShotDownFixture,
       AShotFliesThroughAMissileThatIsNotShootable) {
  static_cast<void>(enemyMissileAt(WHERE_THE_PLAYER_SAW_THE_ENEMY));
  static_cast<void>(shotAt(WHERE_THE_PLAYER_SAW_THE_ENEMY, 0));

  runTicks(1);

  EXPECT_TRUE(hits().empty());
}
