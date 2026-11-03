#include <gtest/gtest.h>
#include "InterpolationTestConstants.hpp"
#include "PositionInterpolationFixture.hpp"

using rtype::client::testing::FIRST_STATE_TICK;
using rtype::client::testing::HALFWAY_BETWEEN_THE_SAMPLES;
using rtype::client::testing::PositionInterpolationFixture;
using rtype::client::testing::SAMPLE_AFTER;
using rtype::client::testing::SAMPLE_BEFORE;
using rtype::client::testing::WHERE_THE_SERVER_PUT_IT;

/**
 * Given an entity with samples at ticks 48 and 60 and a clock showing tick 54
 * When the entities are placed
 * Then the entity is halfway between its two samples
 */
TEST_F(PositionInterpolationFixture, PutsAnEntityBetweenItsTwoSamples) {
  record(SAMPLE_BEFORE);
  record(SAMPLE_AFTER);
  receiveStateOfTick(FIRST_STATE_TICK);

  place();

  EXPECT_FLOAT_EQ(position().x, HALFWAY_BETWEEN_THE_SAMPLES.x);
  EXPECT_FLOAT_EQ(position().y, HALFWAY_BETWEEN_THE_SAMPLES.y);
}

/**
 * Given an entity with two samples and a clock that received no state
 * When the entities are placed
 * Then the entity stays where it was
 */
TEST_F(PositionInterpolationFixture, MovesNothingBeforeTheFirstState) {
  record(SAMPLE_BEFORE);
  record(SAMPLE_AFTER);

  place();

  EXPECT_FLOAT_EQ(position().x, WHERE_THE_SERVER_PUT_IT.x);
  EXPECT_FLOAT_EQ(position().y, WHERE_THE_SERVER_PUT_IT.y);
}

/**
 * Given an entity no state listed yet, and a clock showing tick 54
 * When the entities are placed
 * Then the entity stays where it appeared
 */
TEST_F(PositionInterpolationFixture, KeepsTheAppearancePositionWithoutSamples) {
  receiveStateOfTick(FIRST_STATE_TICK);

  place();

  EXPECT_FLOAT_EQ(position().x, WHERE_THE_SERVER_PUT_IT.x);
  EXPECT_FLOAT_EQ(position().y, WHERE_THE_SERVER_PUT_IT.y);
}
