#include <gtest/gtest.h>
#include <string>
#include "GameRuleSettings.hpp"
#include "GameRuleSettingsFixture.hpp"
#include "GameRuleSettingsTestConstants.hpp"
#include "GameRules.hpp"

using rtype::network::defaultGameRules;
using rtype::network::GameRules;
using rtype::server::GameRuleSettings;
using rtype::server::testing::FIVE_LIVES_COUNT;
using rtype::server::testing::HARD_DIFFICULTY_NAME;
using rtype::server::testing::NINE_LIVES_COUNT;
using rtype::server::testing::SET_FIVE_LIVES_LINE;
using rtype::server::testing::SET_FRIENDLY_FIRE_ON_LINE;
using rtype::server::testing::SET_FRIENDLY_FIRE_TWO_LINE;
using rtype::server::testing::SET_HARD_DIFFICULTY_LINE;
using rtype::server::testing::SET_LIVES_PAST_EIGHT_BITS_LINE;
using rtype::server::testing::SET_NINE_LIVES_LINE;
using rtype::server::testing::SET_NO_LIFE_LINE;
using rtype::server::testing::SET_TEN_LIVES_LINE;

/**
 * Given settings of the rules never declared
 * When their rules are read
 * Then they are the default rules
 */
TEST(GameRuleSettings, HoldsTheDefaultRulesBeforeTheyAreDeclared) {
  const GameRuleSettings settings;

  EXPECT_EQ(settings.rules(), defaultGameRules());
}

/**
 * Given the settings of the rules declared, and no line run
 * When their rules are read
 * Then they are the default rules: normal, three lives, no friendly fire
 */
TEST_F(GameRuleSettingsFixture, HoldsTheDefaultRulesOnceDeclared) {
  EXPECT_EQ(rules(), defaultGameRules());
}

/**
 * Given the settings of the rules declared
 * When lines set the difficulty "hard", five lives and friendly fire on
 * Then the rules are those
 */
TEST_F(GameRuleSettingsFixture, HoldsTheRulesTheLinesSet) {
  execute(SET_HARD_DIFFICULTY_LINE);
  execute(SET_FIVE_LIVES_LINE);
  execute(SET_FRIENDLY_FIRE_ON_LINE);

  EXPECT_EQ(rules(), (GameRules{.difficulty = std::string{HARD_DIFFICULTY_NAME},
                                .lives = FIVE_LIVES_COUNT,
                                .friendlyFire = true}));
}

/**
 * Given the settings of the rules declared
 * When a line sets nine lives, the most a game accepts
 * Then the line is accepted and the rules give nine lives
 */
TEST_F(GameRuleSettingsFixture, AcceptsTheMostLives) {
  EXPECT_TRUE(execute(SET_NINE_LIVES_LINE).accepted);
  EXPECT_EQ(rules().lives, NINE_LIVES_COUNT);
}

/**
 * Given the settings of the rules declared
 * When a line sets ten lives, one past the most
 * Then the line is refused and the rules keep the default lives
 */
TEST_F(GameRuleSettingsFixture, RefusesOneLifePastTheMost) {
  EXPECT_FALSE(execute(SET_TEN_LIVES_LINE).accepted);
  EXPECT_EQ(rules().lives, defaultGameRules().lives);
}

/**
 * Given the settings of the rules declared
 * When a line sets 257 lives, which a cut to 8 bits would read as one
 * Then the line is refused and the rules keep the default lives
 */
TEST_F(GameRuleSettingsFixture, RefusesLivesPastEightBits) {
  EXPECT_FALSE(execute(SET_LIVES_PAST_EIGHT_BITS_LINE).accepted);
  EXPECT_EQ(rules().lives, defaultGameRules().lives);
}

/**
 * Given the settings of the rules declared
 * When a line sets no life
 * Then the line is refused: a player starts with one life at least
 */
TEST_F(GameRuleSettingsFixture, RefusesNoLife) {
  EXPECT_FALSE(execute(SET_NO_LIFE_LINE).accepted);
}

/**
 * Given the settings of the rules declared
 * When a line sets the friendly fire to 2
 * Then the line is refused and friendly fire stays off
 */
TEST_F(GameRuleSettingsFixture, RefusesAFriendlyFireOfTwo) {
  EXPECT_FALSE(execute(SET_FRIENDLY_FIRE_TWO_LINE).accepted);
  EXPECT_FALSE(rules().friendlyFire);
}
