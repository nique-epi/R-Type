#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <random>
#include <ranges>
#include <string>
#include <vector>
#include "FlightLogFixtures.hpp"
#include "FlightLogTestConstants.hpp"
#include "Taunt.hpp"
#include "TauntCondition.hpp"
#include "TauntConstants.hpp"
#include "Taunts.hpp"
#include "TauntsFixtures.hpp"
#include "TauntsTestConstants.hpp"

using rtype::client::pickTaunt;
using rtype::client::Taunt;
using rtype::client::TAUNT_LINE_BREAK;
using rtype::client::TauntCondition;
using rtype::client::tauntFits;
using rtype::client::tauntLinesOf;
using rtype::client::TAUNTS;
using rtype::client::testing::ABOUT_TWENTY;
using rtype::client::testing::BOTTOM_SCORE;
using rtype::client::testing::LABEL_LINE_LETTERS;
using rtype::client::testing::SECOND_SCORE;
using rtype::client::testing::SEEDS_TRIED;
using rtype::client::testing::SMALL_SCORE;
using rtype::client::testing::TOP_SCORE;
using rtype::client::testing::UNCONDITIONAL_AT_LEAST;
using rtype::client::testing::ZERO_SCORE;

/**
 * Given the taunts of the cover
 * When they are counted
 * Then there are about twenty, as asked
 */
TEST(Taunts, ThereAreAboutTwenty) { EXPECT_GE(TAUNTS.size(), ABOUT_TWENTY); }

/**
 * Given every taunt, its names the longest a nickname can be
 * When its lines are measured
 * Then each holds 18 characters at most, so it fits the label
 */
TEST(Taunts, EveryLineFitsTheLabel) {
  for (const Taunt& taunt : TAUNTS) {
    for (const auto line : std::views::split(taunt.text, TAUNT_LINE_BREAK)) {
      const std::string named =
          withLongestName(std::string(line.begin(), line.end()));
      SCOPED_TRACE(named);
      EXPECT_LE(named.size(), LABEL_LINE_LETTERS);
    }
  }
}

/**
 * Given any game
 * When a taunt is drawn
 * Then at least two taunts fit it, so one remains besides the previous
 */
TEST(Taunts, TwoTauntsAtLeastFitAnyGame) {
  const auto unconditional =
      std::ranges::count(TAUNTS, TauntCondition::Always, &Taunt::condition);
  EXPECT_GE(static_cast<std::size_t>(unconditional), UNCONDITIONAL_AT_LEAST);
}

/**
 * Given a previous taunt and 200 different seeds
 * When a taunt is drawn with each
 * Then the previous taunt is never drawn
 */
TEST(Taunts, TheNextTauntIsNeverThePrevious) {
  const auto pilots = pilotsScoring({TOP_SCORE, SECOND_SCORE, BOTTOM_SCORE});
  for (std::uint32_t seed = 0; seed < SEEDS_TRIED; ++seed) {
    std::mt19937 random(seed);
    const std::size_t previous = seed % TAUNTS.size();
    EXPECT_NE(pickTaunt(pilots, random, previous), previous);
  }
}

/**
 * Given three pilots tied on the same score
 * When the taunts naming the best or the last are tried
 * Then they do not fit: nobody carried anyone
 */
TEST(Taunts, CarryingNeedsTheBestAheadOfTheLast) {
  const auto tied = pilotsScoring({BOTTOM_SCORE, BOTTOM_SCORE, BOTTOM_SCORE});
  EXPECT_FALSE(tauntFits(
      Taunt{.text = "{BEST}", .condition = TauntCondition::BestAheadOfLast},
      tied));
}

/**
 * Given a pilot alone, then two pilots
 * When the taunt of a lone pilot is tried
 * Then it fits the lone pilot only
 */
TEST(Taunts, TheSoloTauntFitsALonePilotOnly) {
  const Taunt solo{.text = "SOLO", .condition = TauntCondition::Solo};
  EXPECT_TRUE(tauntFits(solo, pilotsScoring({BOTTOM_SCORE})));
  EXPECT_FALSE(tauntFits(solo, pilotsScoring({BOTTOM_SCORE, SMALL_SCORE})));
}

/**
 * Given pilots who all scored, then one who scored 0
 * When the taunt of a score of 0 is tried
 * Then it fits only when a pilot scored 0
 */
TEST(Taunts, TheZeroTauntNeedsAPilotAtZero) {
  const Taunt zero{.text = "{ZERO}", .condition = TauntCondition::ZeroScore};
  EXPECT_FALSE(tauntFits(zero, pilotsScoring({BOTTOM_SCORE, SMALL_SCORE})));
  EXPECT_TRUE(tauntFits(zero, pilotsScoring({BOTTOM_SCORE, ZERO_SCORE})));
}

/**
 * Given pilots ranked best first
 * When a taunt naming the best is written
 * Then its first line is the nickname of the best
 */
TEST(Taunts, TheBestPilotIsNamed) {
  auto pilots = pilotsScoring({TOP_SCORE, BOTTOM_SCORE});
  pilots[0].nickname = "CAPTAIN_STARDUST";
  const std::vector<std::string> lines =
      tauntLinesOf(Taunt{.text = "{BEST}\nCARRIED THE TEAM",
                         .condition = TauntCondition::BestAheadOfLast},
                   pilots);

  ASSERT_EQ(lines.size(), 2U);
  EXPECT_EQ(lines[0], "CAPTAIN_STARDUST");
}

/**
 * Given a last pilot without a nickname, number 2
 * When a taunt naming the last is written
 * Then the pilot is named P2
 */
TEST(Taunts, APilotWithoutNicknameIsNamedByItsNumber) {
  auto pilots = pilotsScoring({TOP_SCORE, BOTTOM_SCORE});
  pilots[1].nickname.clear();
  const std::vector<std::string> lines = tauntLinesOf(
      Taunt{.text = "{LAST}", .condition = TauntCondition::BestAheadOfLast},
      pilots);

  ASSERT_EQ(lines.size(), 1U);
  EXPECT_EQ(lines[0], "P2");
}
