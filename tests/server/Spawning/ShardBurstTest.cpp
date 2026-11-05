#include <gtest/gtest.h>
#include <cmath>
#include <cstdint>
#include <set>
#include <utility>
#include <vector>
#include "BodyPlacement.hpp"
#include "EntityDestruction.hpp"
#include "EntitySheetsTestConstants.hpp"
#include "ShardBurstFixture.hpp"
#include "ShardBurstTestConstants.hpp"

using rtype::game::BodyPlacement;
using rtype::game::DestructionCause;
using rtype::game::EntityDestruction;
using rtype::game::testing::ORB_BURST_TICKS;
using rtype::game::testing::SHARDS_PER_ORB;
using rtype::server::testing::ORB_CENTER;
using rtype::server::testing::SHARD_TOLERANCE_UNITS;
using rtype::server::testing::ShardBurstFixture;

/**
 * Given an orb that bursts after 6 ticks
 * When 5 ticks run
 * Then it is still whole: no shard appeared and nothing was removed
 */
TEST_F(ShardBurstFixture, AnOrbStaysWholeUntilItsTime) {
  static_cast<void>(orbAt(ORB_CENTER));

  runTicks(ORB_BURST_TICKS - 1);

  EXPECT_EQ(shards().size(), 1U);
  EXPECT_TRUE(removals().empty());
}

/**
 * Given an orb that bursts after 6 ticks
 * When 6 ticks run
 * Then the orb is removed and announced as removed
 */
TEST_F(ShardBurstFixture, AnOrbIsRemovedWhenItBursts) {
  const std::uint32_t orb = orbAt(ORB_CENTER);

  runTicks(ORB_BURST_TICKS);

  const std::vector<EntityDestruction> expected{
      {.networkId = orb, .cause = DestructionCause::Removed}};
  EXPECT_EQ(removals(), expected);
}

/**
 * Given an orb that bursts into four shards
 * When it bursts
 * Then four shards steer from its center, one along each diagonal
 */
TEST_F(ShardBurstFixture, TheShardsFlyAwayAlongTheFourDiagonals) {
  static_cast<void>(orbAt(ORB_CENTER));

  runTicks(ORB_BURST_TICKS);

  const std::vector<BodyPlacement> found = shards();
  ASSERT_EQ(found.size(), SHARDS_PER_ORB);
  std::set<std::pair<bool, bool>> ways;
  for (const BodyPlacement& shard : found) {
    const float across = shard.position.x - ORB_CENTER.x;
    const float down = shard.position.y - ORB_CENTER.y;
    EXPECT_FLOAT_EQ(shard.origin.x, ORB_CENTER.x);
    EXPECT_FLOAT_EQ(shard.origin.y, ORB_CENTER.y);
    EXPECT_NEAR(std::abs(across), std::abs(down), SHARD_TOLERANCE_UNITS);
    ways.emplace(across > 0.0F, down > 0.0F);
  }
  EXPECT_EQ(ways.size(), SHARDS_PER_ORB);
}

/**
 * Given a connected player and an orb that bursts into four shards
 * When it bursts
 * Then the player is told about four new entities
 */
TEST_F(ShardBurstFixture, ThePlayersAreToldAboutEachShard) {
  connectPlayer();
  static_cast<void>(orbAt(ORB_CENTER));

  runTicks(ORB_BURST_TICKS);

  EXPECT_EQ(spawnsSent().size(), SHARDS_PER_ORB);
}
