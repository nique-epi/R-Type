#include <gtest/gtest.h>
#include <span>
#include "BehaviourSetting.hpp"
#include "BehaviourTestConstants.hpp"
#include "BehaviourView.hpp"
#include "DirectionConstants.hpp"
#include "Intent.hpp"
#include "WeaveBehaviour.hpp"

using rtype::game::BehaviourSetting;
using rtype::game::BehaviourView;
using rtype::game::FULL_STEERING;
using rtype::game::Intent;
using rtype::game::WeaveBehaviour;
using rtype::game::testing::AMPLITUDE_KEY_IN_A_SHEET;
using rtype::game::testing::BODY_TOP_SPEED;
using rtype::game::testing::HALF_DEPTH_QUARTER_PERIOD_VERTICAL_SHARE;
using rtype::game::testing::HALF_DEPTH_WEAVE_SETTINGS;
using rtype::game::testing::HIGHEST_AMPLITUDE;
using rtype::game::testing::HIGHEST_PERIOD_MS;
using rtype::game::testing::LOWEST_AMPLITUDE;
using rtype::game::testing::LOWEST_PERIOD_MS;
using rtype::game::testing::PERIOD_KEY_IN_A_SHEET;
using rtype::game::testing::QUARTER_PERIOD;
using rtype::game::testing::QUARTER_PERIOD_VERTICAL_SHARE;
using rtype::game::testing::ROW_START;
using rtype::game::testing::SHARE_TOLERANCE;
using rtype::game::testing::WEAVER_SETTINGS;

/**
 * Given a body on its row at 240 units per second, weaving 48 units every
 * 2000 ms, whose next step of half a second ends a quarter period after it
 * appeared
 * When the weave behaviour steers it
 * Then it asks for the share that lands 48 units below the row, the bottom of
 * the weave
 */
TEST(WeaveBehaviour, SteersToTheBottomOfItsWeaveAtAQuarterPeriod) {
  const WeaveBehaviour weave;

  const Intent intent = weave.steer(BehaviourView{.position = ROW_START,
                                                  .origin = ROW_START,
                                                  .age = QUARTER_PERIOD,
                                                  .elapsed = QUARTER_PERIOD,
                                                  .topSpeed = BODY_TOP_SPEED,
                                                  .settings = WEAVER_SETTINGS});

  EXPECT_NEAR(intent.direction.y, QUARTER_PERIOD_VERTICAL_SHARE,
              SHARE_TOLERANCE);
}

/**
 * Given the same body, with a weave of 24 units every 2000 ms
 * When the weave behaviour steers it a quarter period after it appeared
 * Then it asks for half the share, which lands 24 units below the row: the
 * amplitude comes from the sheet
 */
TEST(WeaveBehaviour, AHalfAmplitudeSteersHalfAsDeep) {
  const WeaveBehaviour weave;

  const Intent intent =
      weave.steer(BehaviourView{.position = ROW_START,
                                .origin = ROW_START,
                                .age = QUARTER_PERIOD,
                                .elapsed = QUARTER_PERIOD,
                                .topSpeed = BODY_TOP_SPEED,
                                .settings = HALF_DEPTH_WEAVE_SETTINGS});

  EXPECT_NEAR(intent.direction.y, HALF_DEPTH_QUARTER_PERIOD_VERTICAL_SHARE,
              SHARE_TOLERANCE);
}

/**
 * Given a body on its row, weaving as a Weaver
 * When the weave behaviour steers it
 * Then it asks for the full speed to the left
 */
TEST(WeaveBehaviour, CrossesLeftAtFullSpeed) {
  const WeaveBehaviour weave;

  const Intent intent = weave.steer(BehaviourView{.position = ROW_START,
                                                  .origin = ROW_START,
                                                  .age = QUARTER_PERIOD,
                                                  .elapsed = QUARTER_PERIOD,
                                                  .topSpeed = BODY_TOP_SPEED,
                                                  .settings = WEAVER_SETTINGS});

  EXPECT_EQ(intent.direction.x, -FULL_STEERING);
}

/**
 * Given a body on its row, weaving as a Weaver
 * When the weave behaviour steers it
 * Then it wants to shoot
 */
TEST(WeaveBehaviour, WantsToShoot) {
  const WeaveBehaviour weave;

  const Intent intent = weave.steer(BehaviourView{.position = ROW_START,
                                                  .origin = ROW_START,
                                                  .age = QUARTER_PERIOD,
                                                  .elapsed = QUARTER_PERIOD,
                                                  .topSpeed = BODY_TOP_SPEED,
                                                  .settings = WEAVER_SETTINGS});

  EXPECT_TRUE(intent.shooting);
}

/**
 * Given the weave behaviour
 * When its settings are asked for
 * Then it declares the amplitude, 0 to 48 units, the room every row keeps,
 * then the period, 1 to 60000 ms
 */
TEST(WeaveBehaviour, DeclaresItsAmplitudeThenItsPeriod) {
  const WeaveBehaviour weave;

  const std::span<const BehaviourSetting> settings = weave.settings();

  ASSERT_EQ(settings.size(), 2U);
  EXPECT_EQ(settings.front().name, AMPLITUDE_KEY_IN_A_SHEET);
  EXPECT_EQ(settings.front().range.lowest, LOWEST_AMPLITUDE);
  EXPECT_EQ(settings.front().range.highest, HIGHEST_AMPLITUDE);
  EXPECT_EQ(settings.back().name, PERIOD_KEY_IN_A_SHEET);
  EXPECT_EQ(settings.back().range.lowest, LOWEST_PERIOD_MS);
  EXPECT_EQ(settings.back().range.highest, HIGHEST_PERIOD_MS);
}
