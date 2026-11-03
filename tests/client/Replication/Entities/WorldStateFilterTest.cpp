#include <gtest/gtest.h>
#include "WorldStateFilter.hpp"
#include "WorldStateFilterTestConstants.hpp"

using rtype::client::WorldStateFilter;
using rtype::client::testing::APPLIED_TICK;
using rtype::client::testing::FIRST_TICK;
using rtype::client::testing::LARGEST_TICK;
using rtype::client::testing::NEWER_TICK;
using rtype::client::testing::OLDER_TICK;
using rtype::client::testing::OLDEST_TICK;

/**
 * Given a filter that has applied nothing
 * When a state arrives, whatever its tick
 * Then it is accepted
 */
TEST(WorldStateFilter, AcceptsTheFirstState) {
  WorldStateFilter filter;

  EXPECT_TRUE(filter.accept(FIRST_TICK));
}

/**
 * Given a filter that applied a tick
 * When a newer tick arrives
 * Then it is accepted
 */
TEST(WorldStateFilter, AcceptsANewerState) {
  WorldStateFilter filter;
  EXPECT_TRUE(filter.accept(APPLIED_TICK));

  EXPECT_TRUE(filter.accept(NEWER_TICK));
}

/**
 * Given a filter that applied a tick, then a newer one
 * When the first tick arrives again
 * Then it is refused: the newer tick became the reference
 */
TEST(WorldStateFilter, RemembersTheNewestTickAccepted) {
  WorldStateFilter filter;
  EXPECT_TRUE(filter.accept(APPLIED_TICK));
  EXPECT_TRUE(filter.accept(NEWER_TICK));

  EXPECT_FALSE(filter.accept(APPLIED_TICK));
}

/**
 * Given a filter that applied a tick
 * When the same tick arrives again, as another part of the same state
 * Then it is accepted
 */
TEST(WorldStateFilter, AcceptsAnotherPartOfTheSameState) {
  WorldStateFilter filter;
  EXPECT_TRUE(filter.accept(APPLIED_TICK));

  EXPECT_TRUE(filter.accept(APPLIED_TICK));
}

/**
 * Given a filter that applied a tick
 * When an older tick arrives
 * Then it is refused
 */
TEST(WorldStateFilter, RefusesAnOlderState) {
  WorldStateFilter filter;
  EXPECT_TRUE(filter.accept(APPLIED_TICK));

  EXPECT_FALSE(filter.accept(OLDER_TICK));
}

/**
 * Given a filter that applied a tick, then refused a much older one
 * When a tick between the two arrives
 * Then it is refused: the refused state did not lower the newest tick
 */
TEST(WorldStateFilter, KeepsTheNewestTickAfterARefusal) {
  WorldStateFilter filter;
  EXPECT_TRUE(filter.accept(APPLIED_TICK));
  EXPECT_FALSE(filter.accept(OLDEST_TICK));

  EXPECT_FALSE(filter.accept(OLDER_TICK));
}

/**
 * Given a filter that applied the largest tick a 32-bit counter holds
 * When tick 0 arrives
 * Then it is refused: ticks are not compared with wrap-around, which takes
 * 2.27 years of play at 60 ticks per second
 */
TEST(WorldStateFilter, DoesNotTreatAWrappedTickAsNewer) {
  WorldStateFilter filter;
  EXPECT_TRUE(filter.accept(LARGEST_TICK));

  EXPECT_FALSE(filter.accept(FIRST_TICK));
}
