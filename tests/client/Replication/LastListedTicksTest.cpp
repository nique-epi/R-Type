#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "EntityReplicatorTestConstants.hpp"
#include "LastListedTicks.hpp"
#include "LastListedTicksTestConstants.hpp"

using rtype::client::LastListedTicks;
using rtype::client::testing::AFTER_REMOVAL_TICK;
using rtype::client::testing::BYDO_ID;
using rtype::client::testing::LAST_KEPT_TICK;
using rtype::client::testing::LISTING_TICK;
using rtype::client::testing::REMOVAL_TICK;
using rtype::client::testing::SHIP_ID;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given a Bydo listed at LISTING_TICK, and a state of LAST_KEPT_TICK, 53 ticks
 * later
 * When the first part of a state of REMOVAL_TICK leaves it out
 * Then it is not taken: a later part of that state may list it, and the newest
 * complete state comes less than 900 ms after its listing
 */
TEST(LastListedTicks, JudgesAnEntityOnTheNewestCompleteStateOnly) {
  LastListedTicks ticks;
  ticks.listed(BYDO_ID, LISTING_TICK);
  static_cast<void>(ticks.takeUnlistedAt(LAST_KEPT_TICK));

  EXPECT_THAT(ticks.takeUnlistedAt(REMOVAL_TICK), IsEmpty());
}

/**
 * Given a Bydo listed at LISTING_TICK, a state of LAST_KEPT_TICK, and a first
 * part of the state of REMOVAL_TICK that leaves it out
 * When a second part of that state leaves it out too
 * Then it is not taken: a part never completes the state of its own tick
 */
TEST(LastListedTicks, NeverCompletesAStateOnAnotherPartOfItsTick) {
  LastListedTicks ticks;
  ticks.listed(BYDO_ID, LISTING_TICK);
  static_cast<void>(ticks.takeUnlistedAt(LAST_KEPT_TICK));
  static_cast<void>(ticks.takeUnlistedAt(REMOVAL_TICK));

  EXPECT_THAT(ticks.takeUnlistedAt(REMOVAL_TICK), IsEmpty());
}

/**
 * Given a Bydo and a ship listed at LISTING_TICK, and a state of REMOVAL_TICK,
 * 54 ticks later, that lists the ship only
 * When a state of a newer tick shows that state complete
 * Then the Bydo is taken, and the ship is not
 */
TEST(LastListedTicks, TakesTheEntitiesNoCompleteStateListedFor900Milliseconds) {
  LastListedTicks ticks;
  ticks.listed(BYDO_ID, LISTING_TICK);
  ticks.listed(SHIP_ID, LISTING_TICK);
  static_cast<void>(ticks.takeUnlistedAt(LISTING_TICK));
  ticks.listed(SHIP_ID, REMOVAL_TICK);
  static_cast<void>(ticks.takeUnlistedAt(REMOVAL_TICK));

  EXPECT_THAT(ticks.takeUnlistedAt(AFTER_REMOVAL_TICK), ElementsAre(BYDO_ID));
}

/**
 * Given a Bydo listed at LISTING_TICK, then again at LAST_KEPT_TICK, and a
 * state of REMOVAL_TICK that leaves it out
 * When a state of a newer tick shows that state complete
 * Then it is not taken: it counts from the newest state that listed it
 */
TEST(LastListedTicks, CountsFromTheNewestStateListingAnEntity) {
  LastListedTicks ticks;
  ticks.listed(BYDO_ID, LISTING_TICK);
  ticks.listed(BYDO_ID, LAST_KEPT_TICK);
  static_cast<void>(ticks.takeUnlistedAt(REMOVAL_TICK));

  EXPECT_THAT(ticks.takeUnlistedAt(AFTER_REMOVAL_TICK), IsEmpty());
}

/**
 * Given a Bydo taken once the state of REMOVAL_TICK was complete
 * When the next state leaves it out too
 * Then it is not taken a second time
 */
TEST(LastListedTicks, TakesAnEntityOnce) {
  LastListedTicks ticks;
  ticks.listed(BYDO_ID, LISTING_TICK);
  static_cast<void>(ticks.takeUnlistedAt(REMOVAL_TICK));
  static_cast<void>(ticks.takeUnlistedAt(AFTER_REMOVAL_TICK));

  EXPECT_THAT(ticks.takeUnlistedAt(AFTER_REMOVAL_TICK + 1), IsEmpty());
}

/**
 * Given a Bydo listed at LISTING_TICK, then forgotten as the client destroyed
 * it, and a state of REMOVAL_TICK
 * When a state of a newer tick shows that state complete
 * Then it is not taken
 */
TEST(LastListedTicks, NeverTakesAForgottenEntity) {
  LastListedTicks ticks;
  ticks.listed(BYDO_ID, LISTING_TICK);
  ticks.forget(BYDO_ID);
  static_cast<void>(ticks.takeUnlistedAt(REMOVAL_TICK));

  EXPECT_THAT(ticks.takeUnlistedAt(AFTER_REMOVAL_TICK), IsEmpty());
}

/**
 * Given a state of LISTING_TICK, then a Bydo that appeared through an
 * EntitySpawned, and a state of LAST_KEPT_TICK
 * When a state of a newer tick shows that state complete
 * Then it is not taken: it counts from the newest state before it appeared
 */
TEST(LastListedTicks, KeepsAnAppearedEntityLessThan900MillisecondsAfterIt) {
  LastListedTicks ticks;
  static_cast<void>(ticks.takeUnlistedAt(LISTING_TICK));
  ticks.appeared(BYDO_ID);
  static_cast<void>(ticks.takeUnlistedAt(LAST_KEPT_TICK));

  EXPECT_THAT(ticks.takeUnlistedAt(REMOVAL_TICK), IsEmpty());
}

/**
 * Given a state of LISTING_TICK, then a Bydo that appeared through an
 * EntitySpawned, and a state of REMOVAL_TICK that leaves it out
 * When a state of a newer tick shows that state complete
 * Then it is taken
 */
TEST(LastListedTicks, TakesAnAppearedEntityNoStateListed) {
  LastListedTicks ticks;
  static_cast<void>(ticks.takeUnlistedAt(LISTING_TICK));
  ticks.appeared(BYDO_ID);
  static_cast<void>(ticks.takeUnlistedAt(REMOVAL_TICK));

  EXPECT_THAT(ticks.takeUnlistedAt(AFTER_REMOVAL_TICK), ElementsAre(BYDO_ID));
}

/**
 * Given a Bydo that appeared before any state, and a first state, of
 * LISTING_TICK, that leaves it out
 * When a state of REMOVAL_TICK shows that first state complete
 * Then it is not taken: it starts counting at the first state, not at the
 * start of the game
 */
TEST(LastListedTicks, StartsCountingAnEntityThatAppearedFirstAtTheFirstState) {
  LastListedTicks ticks;
  ticks.appeared(BYDO_ID);
  static_cast<void>(ticks.takeUnlistedAt(LISTING_TICK));

  EXPECT_THAT(ticks.takeUnlistedAt(REMOVAL_TICK), IsEmpty());
}

/**
 * Given a Bydo that appeared before any state, a first state of LISTING_TICK
 * and a state of REMOVAL_TICK, both leaving it out
 * When a state of a newer tick shows the state of REMOVAL_TICK complete
 * Then it is taken
 */
TEST(LastListedTicks, TakesAnEntityThatAppearedFirstOnceNoStateListedIt) {
  LastListedTicks ticks;
  ticks.appeared(BYDO_ID);
  static_cast<void>(ticks.takeUnlistedAt(LISTING_TICK));
  static_cast<void>(ticks.takeUnlistedAt(REMOVAL_TICK));

  EXPECT_THAT(ticks.takeUnlistedAt(AFTER_REMOVAL_TICK), ElementsAre(BYDO_ID));
}
