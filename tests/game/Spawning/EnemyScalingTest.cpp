#include <gtest/gtest.h>
#include "BossHealthScalingTestConstants.hpp"
#include "EnemyScaling.hpp"
#include "EnemyScalingTestConstants.hpp"
#include "EnemyStrength.hpp"

using rtype::game::EnemyStrength;
using rtype::game::scaledEnemyStrength;
using rtype::game::threatenedEnemyStrength;
using rtype::game::testing::FOUR_PLAYERS;
using rtype::game::testing::HALF_THE_MOST_THREAT;
using rtype::game::testing::HARD_GUN_INTERVAL_AT_THE_MOST_THREAT;
using rtype::game::testing::HARD_GUN_INTERVAL_FOR_FOUR;
using rtype::game::testing::HARD_HEALTH_AT_HALF_THE_MOST_THREAT;
using rtype::game::testing::HARD_HEALTH_AT_THE_MOST_THREAT;
using rtype::game::testing::HARD_HEALTH_FOR_FOUR;
using rtype::game::testing::HARD_STRENGTH;
using rtype::game::testing::NO_PLAYER;
using rtype::game::testing::NO_THREAT;
using rtype::game::testing::ONE_PLAYER;
using rtype::game::testing::THE_MOST_THREAT;

/**
 * Given the strength of hard and a game with one player
 * When the strength of its enemies is scaled
 * Then it is the strength of hard, untouched
 */
TEST(EnemyScaling, OnePlayerFacesTheStrengthOfTheDifficulty) {
  EXPECT_EQ(scaledEnemyStrength(HARD_STRENGTH, ONE_PLAYER), HARD_STRENGTH);
}

/**
 * Given the strength of hard and a team of four
 * When the strength of its enemies is scaled
 * Then their health rises 40 % per player past the first, to 264 %
 */
TEST(EnemyScaling, EachPlayerPastTheFirstRaisesTheHealth) {
  EXPECT_EQ(scaledEnemyStrength(HARD_STRENGTH, FOUR_PLAYERS).healthPercent,
            HARD_HEALTH_FOR_FOUR);
}

/**
 * Given the strength of hard and a team of four
 * When the strength of its enemies is scaled
 * Then their guns wait 10 % fewer ticks per player past the first, 56 %
 */
TEST(EnemyScaling, EachPlayerPastTheFirstShortensTheGuns) {
  EXPECT_EQ(scaledEnemyStrength(HARD_STRENGTH, FOUR_PLAYERS).gunIntervalPercent,
            HARD_GUN_INTERVAL_FOR_FOUR);
}

/**
 * Given the strength of hard and a game with nobody connected
 * When the strength of its enemies is scaled
 * Then it is the strength for one player, never weaker than the difficulty
 */
TEST(EnemyScaling, NoPlayerCountsAsOne) {
  EXPECT_EQ(scaledEnemyStrength(HARD_STRENGTH, NO_PLAYER), HARD_STRENGTH);
}

/**
 * Given the strength of hard
 * When it answers no threat
 * Then it stays the strength of hard
 */
TEST(EnemyScaling, NoThreatLeavesTheStrength) {
  EXPECT_EQ(threatenedEnemyStrength(HARD_STRENGTH, NO_THREAT), HARD_STRENGTH);
}

/**
 * Given the strength of hard
 * When it answers the most threat
 * Then the health rises 50 %, to 180 %
 */
TEST(EnemyScaling, TheMostThreatRaisesTheHealthByHalf) {
  EXPECT_EQ(
      threatenedEnemyStrength(HARD_STRENGTH, THE_MOST_THREAT).healthPercent,
      HARD_HEALTH_AT_THE_MOST_THREAT);
}

/**
 * Given the strength of hard
 * When it answers half the most threat
 * Then the health rises by half of 50 %, to 150 %
 */
TEST(EnemyScaling, HalfTheMostThreatRaisesTheHealthByAQuarter) {
  EXPECT_EQ(threatenedEnemyStrength(HARD_STRENGTH, HALF_THE_MOST_THREAT)
                .healthPercent,
            HARD_HEALTH_AT_HALF_THE_MOST_THREAT);
}

/**
 * Given the strength of hard
 * When it answers the most threat
 * Then the guns wait 23 % fewer ticks, 61 %
 */
TEST(EnemyScaling, TheMostThreatShortensTheGuns) {
  EXPECT_EQ(threatenedEnemyStrength(HARD_STRENGTH, THE_MOST_THREAT)
                .gunIntervalPercent,
            HARD_GUN_INTERVAL_AT_THE_MOST_THREAT);
}
