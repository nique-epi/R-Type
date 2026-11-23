#include <gtest/gtest.h>
#include "ThreatRank.hpp"
#include "ThreatRankFixtures.hpp"
#include "ThreatRankTestConstants.hpp"

using rtype::game::ThreatRank;
using rtype::game::testing::A_QUARTER;
using rtype::game::testing::ALMOST_THE_MOST;
using rtype::game::testing::HALF_RISE_TICKS;
using rtype::game::testing::HALF_THREAT;
using rtype::game::testing::JUST_UNDER_A_QUARTER;
using rtype::game::testing::levelAfter;
using rtype::game::testing::LONGER_THAN_THE_RISE;
using rtype::game::testing::NO_THREAT;
using rtype::game::testing::ONE_TICK;
using rtype::game::testing::TEAM_A_QUARTER_OF_THE_WAY;
using rtype::game::testing::TEAM_JUST_UNDER_A_QUARTER;
using rtype::game::testing::TEAM_PAST_THE_STRONGEST;
using rtype::game::testing::TEAM_WITH_ITS_GUNS;
using rtype::game::testing::TEAM_WITH_THE_STRONGEST;
using rtype::game::testing::TEAM_WITHOUT_WEAPONS;
using rtype::game::testing::THE_MOST;
using rtype::game::testing::WHOLE_RISE_TICKS;

/**
 * Given a team that fires the guns of its sheets
 * When the threat follows it for longer than its rise
 * Then there is no threat
 */
TEST(ThreatRank, AGunAloneMakesNoThreat) {
  ThreatRank rank;

  EXPECT_EQ(levelAfter(rank, TEAM_WITH_ITS_GUNS, LONGER_THAN_THE_RISE),
            NO_THREAT);
}

/**
 * Given a team that fires its strongest weapon
 * When the threat follows it for 4 s
 * Then it is half the most, tier 2
 */
TEST(ThreatRank, TheThreatRisesHalfWayInHalfTheRise) {
  ThreatRank rank;

  EXPECT_EQ(levelAfter(rank, TEAM_WITH_THE_STRONGEST, HALF_RISE_TICKS),
            HALF_THREAT);
}

/**
 * Given a team that fires its strongest weapon
 * When the threat follows it for one tick less than 8 s
 * Then it is just under the most
 */
TEST(ThreatRank, TheThreatIsUnderTheMostBeforeEightSeconds) {
  ThreatRank rank;

  EXPECT_EQ(levelAfter(rank, TEAM_WITH_THE_STRONGEST, WHOLE_RISE_TICKS - 1),
            ALMOST_THE_MOST);
}

/**
 * Given a team that fires its strongest weapon
 * When the threat follows it for 8 s
 * Then it is the most, tier 3
 */
TEST(ThreatRank, TheThreatReachesTheMostInEightSeconds) {
  ThreatRank rank;

  EXPECT_EQ(levelAfter(rank, TEAM_WITH_THE_STRONGEST, WHOLE_RISE_TICKS),
            THE_MOST);
}

/**
 * Given a threat at the most
 * When its team goes back to its guns for one tick
 * Then there is no threat left
 */
TEST(ThreatRank, TheThreatFallsAtOnce) {
  ThreatRank rank;
  static_cast<void>(
      levelAfter(rank, TEAM_WITH_THE_STRONGEST, WHOLE_RISE_TICKS));

  EXPECT_EQ(levelAfter(rank, TEAM_WITH_ITS_GUNS, ONE_TICK), NO_THREAT);
}

/**
 * Given a team that fires more than its strongest weapon
 * When the threat follows it for longer than its rise
 * Then it stays at the most
 */
TEST(ThreatRank, AFirepowerPastTheStrongestStaysAtTheMost) {
  ThreatRank rank;

  EXPECT_EQ(levelAfter(rank, TEAM_PAST_THE_STRONGEST, LONGER_THAN_THE_RISE),
            THE_MOST);
}

/**
 * Given a team whose sheets hold no weapon stronger than their gun
 * When the threat follows it for longer than its rise
 * Then there is no threat
 */
TEST(ThreatRank, SheetsWithoutWeaponsMakeNoThreat) {
  ThreatRank rank;

  EXPECT_EQ(levelAfter(rank, TEAM_WITHOUT_WEAPONS, LONGER_THAN_THE_RISE),
            NO_THREAT);
}

/**
 * Given a team a quarter of the way from its guns to its strongest weapon
 * When the threat follows it for longer than its rise
 * Then the threat is 25 %, tier 1
 */
TEST(ThreatRank, AQuarterOfTheWayReachesTheFirstTier) {
  ThreatRank rank;

  EXPECT_EQ(levelAfter(rank, TEAM_A_QUARTER_OF_THE_WAY, LONGER_THAN_THE_RISE),
            A_QUARTER);
}

/**
 * Given a team just under a quarter of the way
 * When the threat follows it for longer than its rise
 * Then the threat is 23 %, still tier 0
 */
TEST(ThreatRank, JustUnderAQuarterStaysUnderTheFirstTier) {
  ThreatRank rank;

  EXPECT_EQ(levelAfter(rank, TEAM_JUST_UNDER_A_QUARTER, LONGER_THAN_THE_RISE),
            JUST_UNDER_A_QUARTER);
}
