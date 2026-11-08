#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "ConnectionRefused.hpp"
#include "GameRules.hpp"
#include "RulesRefusal.hpp"
#include "RulesRefusalFixtures.hpp"
#include "RulesRefusalTestConstants.hpp"
#include "SupervisedGamesTestConstants.hpp"

using rtype::network::defaultGameRules;
using rtype::network::GameRules;
using rtype::network::RefusalReason;
using rtype::server::refusalOf;
using rtype::server::testing::rulesWithLives;
using rtype::server::testing::shippedDifficulties;

/**
 * Given the difficulties of the shipped waves file
 * When the default rules are judged
 * Then nothing refuses them
 */
TEST(RulesRefusal, AcceptsTheDefaultRules) {
  EXPECT_EQ(refusalOf(defaultGameRules(), shippedDifficulties()), std::nullopt);
}

/**
 * Given the difficulties of the shipped waves file
 * When rules naming the difficulty "nightmare" are judged
 * Then they are refused as UnknownDifficulty
 */
TEST(RulesRefusal, RefusesADifficultyTheWavesFileHasNot) {
  const GameRules rules{.difficulty = std::string{UNKNOWN_DIFFICULTY},
                        .lives = CHOSEN_LIVES,
                        .friendlyFire = false};

  EXPECT_EQ(refusalOf(rules, shippedDifficulties()),
            RefusalReason::UnknownDifficulty);
}

/**
 * Given the difficulties of the shipped waves file
 * When rules giving no life are judged
 * Then they are refused as LivesOutOfRange
 */
TEST(RulesRefusal, RefusesNoLife) {
  EXPECT_EQ(refusalOf(rulesWithLives(NO_LIFE), shippedDifficulties()),
            RefusalReason::LivesOutOfRange);
}

/**
 * Given the difficulties of the shipped waves file
 * When rules giving one life, the fewest a game accepts, are judged
 * Then nothing refuses them
 */
TEST(RulesRefusal, AcceptsOneLife) {
  EXPECT_EQ(refusalOf(rulesWithLives(ONE_LIFE), shippedDifficulties()),
            std::nullopt);
}

/**
 * Given the difficulties of the shipped waves file
 * When rules giving nine lives, the most a game accepts, are judged
 * Then nothing refuses them
 */
TEST(RulesRefusal, AcceptsNineLives) {
  EXPECT_EQ(refusalOf(rulesWithLives(NINE_LIVES), shippedDifficulties()),
            std::nullopt);
}

/**
 * Given the difficulties of the shipped waves file
 * When rules giving ten lives, one past the most, are judged
 * Then they are refused as LivesOutOfRange
 */
TEST(RulesRefusal, RefusesTenLives) {
  EXPECT_EQ(
      refusalOf(rulesWithLives(LIVES_PAST_THE_MOST), shippedDifficulties()),
      RefusalReason::LivesOutOfRange);
}

/**
 * Given the difficulties of the shipped waves file
 * When rules naming the difficulty "nightmare" and giving ten lives are judged
 * Then the difficulty is the reason given: it is checked first
 */
TEST(RulesRefusal, NamesTheDifficultyBeforeTheLives) {
  const GameRules rules{.difficulty = std::string{UNKNOWN_DIFFICULTY},
                        .lives = LIVES_PAST_THE_MOST,
                        .friendlyFire = false};

  EXPECT_EQ(refusalOf(rules, shippedDifficulties()),
            RefusalReason::UnknownDifficulty);
}
