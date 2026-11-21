#include <gtest/gtest.h>
#include "BossHealthScalingTestConstants.hpp"
#include "EnemyScaling.hpp"
#include "EnemyScalingTestConstants.hpp"
#include "EnemyStrength.hpp"

using rtype::game::EnemyStrength;
using rtype::game::scaledEnemyStrength;
using rtype::game::testing::FOUR_PLAYERS;
using rtype::game::testing::HARD_HEALTH_FOR_FOUR;
using rtype::game::testing::HARD_STRENGTH;
using rtype::game::testing::NO_PLAYER;
using rtype::game::testing::ONE_PLAYER;

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
 * Then their health rises 15 % per player past the first, to 174 %
 */
TEST(EnemyScaling, EachPlayerPastTheFirstRaisesTheHealth) {
  EXPECT_EQ(scaledEnemyStrength(HARD_STRENGTH, FOUR_PLAYERS).healthPercent,
            HARD_HEALTH_FOR_FOUR);
}

/**
 * Given the strength of hard and a team of four
 * When the strength of its enemies is scaled
 * Then their guns keep the interval of hard: more players add no shot
 */
TEST(EnemyScaling, PlayersLeaveTheGunsAsTheDifficultyGivesThem) {
  EXPECT_EQ(scaledEnemyStrength(HARD_STRENGTH, FOUR_PLAYERS).gunIntervalPercent,
            HARD_STRENGTH.gunIntervalPercent);
}

/**
 * Given the strength of hard and a game with nobody connected
 * When the strength of its enemies is scaled
 * Then it is the strength for one player, never weaker than the difficulty
 */
TEST(EnemyScaling, NoPlayerCountsAsOne) {
  EXPECT_EQ(scaledEnemyStrength(HARD_STRENGTH, NO_PLAYER), HARD_STRENGTH);
}
