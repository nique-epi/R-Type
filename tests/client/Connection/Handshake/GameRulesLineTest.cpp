#include <gtest/gtest.h>
#include <string>
#include "GameRules.hpp"
#include "GameRulesLine.hpp"
#include "GameRulesLineTestConstants.hpp"
#include "GameRulesTestConstants.hpp"

using rtype::client::rulesLineOf;
using rtype::client::upperCased;
using rtype::client::testing::LAST_LIFE_LINE;
using rtype::client::testing::LOWER_NAME;
using rtype::client::testing::ONE_LIFE;
using rtype::client::testing::UPPER_NAME;
using rtype::network::defaultGameRules;
using rtype::network::GameRules;
using rtype::testing::CHOSEN_DIFFICULTY;
using rtype::testing::CHOSEN_LIVES;
using rtype::testing::HARD_RULES_LINE;

/**
 * Given a game played hard, with five lives and friendly fire on
 * When its rules are written
 * Then the line names the difficulty in capitals, the lives and the fire
 */
TEST(GameRulesLine, NamesEveryRule) {
  EXPECT_EQ(rulesLineOf(GameRules{.difficulty = std::string(CHOSEN_DIFFICULTY),
                                  .lives = CHOSEN_LIVES,
                                  .friendlyFire = true}),
            HARD_RULES_LINE);
}

/**
 * Given a game of the default difficulty with one life and no friendly fire
 * When its rules are written
 * Then the one life is written in the singular, and the fire as off
 */
TEST(GameRulesLine, WritesOneLifeInTheSingular) {
  GameRules lastLife = defaultGameRules();
  lastLife.lives = ONE_LIFE;

  EXPECT_EQ(rulesLineOf(lastLife), LAST_LIFE_LINE);
}

/**
 * Given the name of a difficulty as the waves file writes it
 * When it is put in capitals
 * Then every letter is a capital
 */
TEST(GameRulesLine, PutsANameInCapitals) {
  EXPECT_EQ(upperCased(LOWER_NAME), UPPER_NAME);
}
