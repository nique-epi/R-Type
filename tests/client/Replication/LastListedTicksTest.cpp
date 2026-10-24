#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "EntityReplicatorTestConstants.hpp"
#include "LastListedTicks.hpp"
#include "LastListedTicksTestConstants.hpp"

using rtype::client::LastListedTicks;
using rtype::client::testing::BYDO_ID;
using rtype::client::testing::LAST_KEPT_TICK;
using rtype::client::testing::LISTING_TICK;
using rtype::client::testing::REMOVAL_TICK;
using rtype::client::testing::SHIP_ID;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given a Bydo listed by a state of LISTING_TICK
 * When a state of LAST_KEPT_TICK, 53 ticks later, leaves it out
 * Then it is not taken: 900 ms have not passed
 */
TEST(LastListedTicks, KeepsAnEntityUnlistedForLessThan900Milliseconds) {
  LastListedTicks ticks;
  ticks.listed(BYDO_ID, LISTING_TICK);

  EXPECT_THAT(ticks.takeUnlistedAt(LAST_KEPT_TICK), IsEmpty());
}

/**
 * Given a Bydo and a ship listed by a state of LISTING_TICK
 * When a state of REMOVAL_TICK, 54 ticks later, lists the ship only
 * Then the Bydo is taken, and the ship is not
 */
TEST(LastListedTicks, TakesTheEntitiesNoStateListedFor900Milliseconds) {
  LastListedTicks ticks;
  ticks.listed(BYDO_ID, LISTING_TICK);
  ticks.listed(SHIP_ID, LISTING_TICK);
  ticks.listed(SHIP_ID, REMOVAL_TICK);

  EXPECT_THAT(ticks.takeUnlistedAt(REMOVAL_TICK), ElementsAre(BYDO_ID));
}

/**
 * Given a Bydo listed at LISTING_TICK, then again at LAST_KEPT_TICK
 * When a state of REMOVAL_TICK leaves it out
 * Then it is not taken: it counts from the newest state that listed it
 */
TEST(LastListedTicks, CountsFromTheNewestStateListingAnEntity) {
  LastListedTicks ticks;
  ticks.listed(BYDO_ID, LISTING_TICK);
  ticks.listed(BYDO_ID, LAST_KEPT_TICK);

  EXPECT_THAT(ticks.takeUnlistedAt(REMOVAL_TICK), IsEmpty());
}

/**
 * Given a Bydo taken by a state of REMOVAL_TICK
 * When the next state leaves it out too
 * Then it is not taken a second time
 */
TEST(LastListedTicks, TakesAnEntityOnce) {
  LastListedTicks ticks;
  ticks.listed(BYDO_ID, LISTING_TICK);
  static_cast<void>(ticks.takeUnlistedAt(REMOVAL_TICK));

  EXPECT_THAT(ticks.takeUnlistedAt(REMOVAL_TICK + 1), IsEmpty());
}

/**
 * Given a Bydo listed at LISTING_TICK, then forgotten as the client destroyed
 * it
 * When a state of REMOVAL_TICK leaves it out
 * Then it is not taken
 */
TEST(LastListedTicks, NeverTakesAForgottenEntity) {
  LastListedTicks ticks;
  ticks.listed(BYDO_ID, LISTING_TICK);
  ticks.forget(BYDO_ID);

  EXPECT_THAT(ticks.takeUnlistedAt(REMOVAL_TICK), IsEmpty());
}

/**
 * Given a state of LISTING_TICK, then a Bydo that appeared through an
 * EntitySpawned
 * When a state of LAST_KEPT_TICK leaves it out
 * Then it is not taken: it counts from the newest state
 */
TEST(LastListedTicks, KeepsAnAppearedEntityLessThan900MillisecondsAfterIt) {
  LastListedTicks ticks;
  static_cast<void>(ticks.takeUnlistedAt(LISTING_TICK));
  ticks.appeared(BYDO_ID);

  EXPECT_THAT(ticks.takeUnlistedAt(LAST_KEPT_TICK), IsEmpty());
}

/**
 * Given a state of LISTING_TICK, then a Bydo that appeared through an
 * EntitySpawned
 * When a state of REMOVAL_TICK leaves it out
 * Then it is taken
 */
TEST(LastListedTicks, TakesAnAppearedEntityNoStateListed) {
  LastListedTicks ticks;
  static_cast<void>(ticks.takeUnlistedAt(LISTING_TICK));
  ticks.appeared(BYDO_ID);

  EXPECT_THAT(ticks.takeUnlistedAt(REMOVAL_TICK), ElementsAre(BYDO_ID));
}

/**
 * Given a Bydo that appeared before any state
 * When the first state, of LISTING_TICK, leaves it out
 * Then it is not taken: it starts counting at that state, not at the start of
 * the game
 */
TEST(LastListedTicks, StartsCountingAnEntityThatAppearedFirstAtTheFirstState) {
  LastListedTicks ticks;
  ticks.appeared(BYDO_ID);

  EXPECT_THAT(ticks.takeUnlistedAt(LISTING_TICK), IsEmpty());
}

/**
 * Given a Bydo that appeared before any state, and a first state of
 * LISTING_TICK that left it out
 * When a state of REMOVAL_TICK leaves it out too
 * Then it is taken
 */
TEST(LastListedTicks, TakesAnEntityThatAppearedFirstOnceNoStateListedIt) {
  LastListedTicks ticks;
  ticks.appeared(BYDO_ID);
  static_cast<void>(ticks.takeUnlistedAt(LISTING_TICK));

  EXPECT_THAT(ticks.takeUnlistedAt(REMOVAL_TICK), ElementsAre(BYDO_ID));
}
