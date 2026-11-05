#include <gtest/gtest.h>
#include <span>
#include "BehaviourSetting.hpp"
#include "BehaviourTestConstants.hpp"
#include "BehaviourView.hpp"
#include "DirectionConstants.hpp"
#include "Intent.hpp"
#include "StraightBehaviour.hpp"

using rtype::game::BehaviourSetting;
using rtype::game::BehaviourView;
using rtype::game::FULL_STEERING;
using rtype::game::Intent;
using rtype::game::StraightBehaviour;
using rtype::game::testing::BODY_TOP_SPEED;
using rtype::game::testing::HEADING_KEY_IN_A_SHEET;
using rtype::game::testing::HIGHEST_HEADING;
using rtype::game::testing::LEFT_HEADING_SETTINGS;
using rtype::game::testing::LOWEST_HEADING;
using rtype::game::testing::QUARTER_PERIOD;
using rtype::game::testing::RIGHT_HEADING_SETTINGS;
using rtype::game::testing::ROW_START;

/**
 * Given a body heading left, a quarter period after it appeared on its row
 * When the straight behaviour steers it
 * Then it asks for the full speed to the left and nothing vertically
 */
TEST(StraightBehaviour, CrossesLeftOnItsRowAtFullSpeed) {
  const StraightBehaviour straight;

  const Intent intent =
      straight.steer(BehaviourView{.position = ROW_START,
                                   .origin = ROW_START,
                                   .age = QUARTER_PERIOD,
                                   .elapsed = QUARTER_PERIOD,
                                   .topSpeed = BODY_TOP_SPEED,
                                   .settings = LEFT_HEADING_SETTINGS});

  EXPECT_EQ(intent.direction.x, -FULL_STEERING);
  EXPECT_EQ(intent.direction.y, 0.0F);
}

/**
 * Given a body heading right, as a missile of a player
 * When the straight behaviour steers it
 * Then it asks for the full speed to the right and nothing vertically
 */
TEST(StraightBehaviour, CrossesRightWithAHeadingOfOne) {
  const StraightBehaviour straight;

  const Intent intent =
      straight.steer(BehaviourView{.position = ROW_START,
                                   .origin = ROW_START,
                                   .age = QUARTER_PERIOD,
                                   .elapsed = QUARTER_PERIOD,
                                   .topSpeed = BODY_TOP_SPEED,
                                   .settings = RIGHT_HEADING_SETTINGS});

  EXPECT_EQ(intent.direction.x, FULL_STEERING);
  EXPECT_EQ(intent.direction.y, 0.0F);
}

/**
 * Given a body that just appeared, heading left
 * When the straight behaviour steers it
 * Then it wants to shoot: how often it fires is the interval of its Gun
 */
TEST(StraightBehaviour, WantsToShoot) {
  const StraightBehaviour straight;

  const Intent intent =
      straight.steer(BehaviourView{.position = ROW_START,
                                   .origin = ROW_START,
                                   .age = QUARTER_PERIOD,
                                   .elapsed = QUARTER_PERIOD,
                                   .topSpeed = BODY_TOP_SPEED,
                                   .settings = LEFT_HEADING_SETTINGS});

  EXPECT_TRUE(intent.shooting);
}

/**
 * Given the straight behaviour
 * When its settings are asked for
 * Then it declares one, heading, from -1 to 1, so a sheet that gives it
 * anything else is refused
 */
TEST(StraightBehaviour, DeclaresItsHeadingFromMinusOneToOne) {
  const StraightBehaviour straight;

  const std::span<const BehaviourSetting> settings = straight.settings();

  ASSERT_EQ(settings.size(), 1U);
  EXPECT_EQ(settings.front().name, HEADING_KEY_IN_A_SHEET);
  EXPECT_EQ(settings.front().range.lowest, LOWEST_HEADING);
  EXPECT_EQ(settings.front().range.highest, HIGHEST_HEADING);
}
