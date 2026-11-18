#include <gtest/gtest.h>
#include <cstddef>
#include "EmissionGroup.hpp"
#include "EmissionGroupFixture.hpp"
#include "EmissionGroupTestConstants.hpp"

using rtype::client::EmissionGroup;

/**
 * Given a group whose first birth comes after 600 ms
 * When 599 ms pass
 * Then no particle is born
 */
TEST_F(EmissionGroupFixture, GivesNoBirthBeforeItsFirstDeadline) {
  EmissionGroup group = skyEvents();

  group.advance(JUST_SHORT_OF_THE_FIRST_EVENT);

  EXPECT_EQ(births(), 0U);
}

/**
 * Given a group whose first birth comes after 600 ms
 * When 601 ms pass
 * Then one particle is born, in one of its members
 */
TEST_F(EmissionGroupFixture, GivesBirthOnceItsFirstDeadlineHasPassed) {
  EmissionGroup group = skyEvents();

  group.advance(JUST_SHORT_OF_THE_FIRST_EVENT);
  group.advance(PAST_THE_FIRST_EVENT);

  EXPECT_EQ(births(), 1U);
  EXPECT_EQ(group.aliveCount(), 1U);
}

/**
 * Given a group that gave its first birth, the next 800 to 2600 ms later
 * When 797 ms pass, then the rest of 2600 ms
 * Then no second particle is born after the first, and one is after the
 * second
 */
TEST_F(EmissionGroupFixture, WaitsItsIntervalBetweenTwoBirths) {
  EmissionGroup group = skyEvents();
  group.advance(JUST_SHORT_OF_THE_FIRST_EVENT);
  group.advance(PAST_THE_FIRST_EVENT);

  group.advance(JUST_SHORT_OF_THE_SHORTEST_GAP);
  const std::size_t beforeTheShortestGap = births();
  group.advance(REST_OF_THE_LONGEST_GAP);

  EXPECT_EQ(beforeTheShortestGap, 1U);
  EXPECT_EQ(births(), 2U);
}

/**
 * Given a group of at most 4 particles, none of which ever ends
 * When a minute passes, a frame at a time
 * Then 4 particles were born, and no more
 */
TEST_F(EmissionGroupFixture, NeverHoldsMoreThanItsCeiling) {
  EmissionGroup group = skyEvents();

  for (int frame = 0; frame < FRAMES_OF_A_MINUTE; ++frame) {
    group.advance(ONE_FRAME_OF_GROUP);
    ASSERT_LE(group.aliveCount(), MOST_EVENTS_AT_ONCE);
  }

  EXPECT_EQ(births(), MOST_EVENTS_AT_ONCE);
}

/**
 * Given a group that holds its 4 particles since long, a birth being due
 * When one of them ends, and a frame passes
 * Then a fifth particle is born at once
 */
TEST_F(EmissionGroupFixture, GivesBirthAsSoonAsAParticleEnds) {
  EmissionGroup group = skyEvents();
  for (int frame = 0; frame < FRAMES_OF_A_MINUTE; ++frame) {
    group.advance(ONE_FRAME_OF_GROUP);
  }
  ASSERT_EQ(births(), MOST_EVENTS_AT_ONCE);

  likely().endOne();
  rare().endOne();
  group.advance(ONE_FRAME_OF_GROUP);

  EXPECT_EQ(births(), MOST_EVENTS_AT_ONCE + 1);
}

/**
 * Given a group of two members, the first four times as likely as the
 * second, whose particles end at once
 * When two thousand particles are born
 * Then about four in five are born in the first
 */
TEST_F(EmissionGroupFixture, DrawsItsMembersByTheirWeights) {
  EmissionGroup group = skyEvents();

  while (births() < TWO_THOUSAND_BIRTHS) {
    group.advance(LONGEST_EVENT_GAP);
    likely().endAll();
    rare().endAll();
  }

  EXPECT_GE(likely().births(), FEWEST_BIRTHS_OF_THE_LIKELY_MEMBER);
  EXPECT_LE(likely().births(), MOST_BIRTHS_OF_THE_LIKELY_MEMBER);
}

/**
 * Given a group without members
 * When a minute passes
 * Then nothing is born, and nothing lives
 */
TEST_F(EmissionGroupFixture, AGroupWithoutMembersGivesNoBirth) {
  EmissionGroup group = skyEventsWithoutMembers();

  for (int frame = 0; frame < FRAMES_OF_A_MINUTE; ++frame) {
    group.advance(ONE_FRAME_OF_GROUP);
  }

  EXPECT_EQ(group.aliveCount(), 0U);
}
