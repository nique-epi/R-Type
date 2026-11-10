#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "CollisionBox.hpp"
#include "Entity.hpp"
#include "Gun.hpp"
#include "Health.hpp"
#include "Intent.hpp"
#include "LuauComponentFieldsTestConstants.hpp"
#include "LuauScriptedBodyFixture.hpp"
#include "Position.hpp"
#include "ScoreValue.hpp"
#include "SteadyMotion.hpp"

using rtype::engine::Entity;
using rtype::game::CollisionBox;
using rtype::game::Gun;
using rtype::game::Health;
using rtype::game::Intent;
using rtype::game::Position;
using rtype::game::ScoreValue;
using rtype::game::SteadyMotion;
using testing::ElementsAre;
using testing::HasSubstr;

/**
 * Given a script that asks for a speed of a hundred times its health
 * When a body with 3 health points of 5 steers with it
 * Then it asks for 300
 */
TEST_F(LuauScriptedBodyFixture, AScriptReadsTheHealthOfItsBody) {
  load(HEALTH_READING_SCRIPT);
  const Entity body = addBody();
  components().add(body, THREE_OF_FIVE_POINTS);

  const Intent intent = steer(FIELD_BEHAVIOUR, body);

  EXPECT_EQ(intent.topSpeedTo.value_or(0.0F), SPEED_AT_THREE_POINTS);
}

/**
 * Given a script that asks for a speed of a hundred times its full health
 * When a body with 3 health points of 5 steers with it
 * Then it asks for 500
 */
TEST_F(LuauScriptedBodyFixture, AScriptReadsTheFullHealthOfItsBody) {
  load(FULL_HEALTH_READING_SCRIPT);
  const Entity body = addBody();
  components().add(body, THREE_OF_FIVE_POINTS);

  const Intent intent = steer(FIELD_BEHAVIOUR, body);

  EXPECT_EQ(intent.topSpeedTo.value_or(0.0F), SPEED_AT_FIVE_FULL_POINTS);
}

/**
 * Given a script that fires when its body has no gun interval
 * When a body without a gun steers with it
 * Then it fires
 */
TEST_F(LuauScriptedBodyFixture, AComponentTheBodyLacksReadsAsNil) {
  load(GUN_ABSENCE_READING_SCRIPT);
  const Entity body = addBody();

  const Intent intent = steer(FIELD_BEHAVIOUR, body);

  EXPECT_TRUE(intent.shooting);
}

/**
 * Given a script that sets the health of its body to 2
 * When a body with 3 points of 5 steers with it
 * Then the body has 2 points
 */
TEST_F(LuauScriptedBodyFixture, AScriptWritesTheHealthOfItsBody) {
  load(HEALTH_WRITING_SCRIPT);
  const Entity body = addBody();
  components().add(body, THREE_OF_FIVE_POINTS);

  static_cast<void>(steer(FIELD_BEHAVIOUR, body));

  const Health* const health = components().get<Health>(body);
  ASSERT_NE(health, nullptr);
  EXPECT_EQ(health->points, TWO_POINTS);
}

/**
 * Given a script that moves its body to (10, 20)
 * When a body at the origin steers with it
 * Then the body is at (10, 20)
 */
TEST_F(LuauScriptedBodyFixture, AScriptWritesThePositionOfItsBody) {
  load(POSITION_WRITING_SCRIPT);
  const Entity body = addBody();
  components().add(body, Position{.x = 0.0F, .y = 0.0F});

  static_cast<void>(steer(FIELD_BEHAVIOUR, body));

  const Position* const position = components().get<Position>(body);
  ASSERT_NE(position, nullptr);
  EXPECT_EQ(position->x, WRITTEN_X);
  EXPECT_EQ(position->y, WRITTEN_Y);
}

/**
 * Given a script that makes the box of its body 40 by 30
 * When a body whose box lies off its center steers with it
 * Then the box is 40 by 30 and keeps its offset
 */
TEST_F(LuauScriptedBodyFixture, AWrittenBoxKeepsItsOffset) {
  load(BOX_WRITING_SCRIPT);
  const Entity body = addBody();
  components().add(body, BOX_OFF_CENTER);

  static_cast<void>(steer(FIELD_BEHAVIOUR, body));

  const CollisionBox* const box = components().get<CollisionBox>(body);
  ASSERT_NE(box, nullptr);
  EXPECT_EQ(box->width, WRITTEN_WIDTH);
  EXPECT_EQ(box->height, WRITTEN_HEIGHT);
  EXPECT_EQ(box->offsetX, BOX_OFF_CENTER.offsetX);
  EXPECT_EQ(box->offsetY, BOX_OFF_CENTER.offsetY);
}

/**
 * Given a script that sets the top speed of its body to 250
 * When a body moving at another speed steers with it
 * Then its motion has that speed
 */
TEST_F(LuauScriptedBodyFixture, AScriptWritesTheTopSpeedOfItsBody) {
  load(TOP_SPEED_WRITING_SCRIPT);
  const Entity body = addBody();
  components().add(body, SteadyMotion{.topSpeed = SPEED_AT_THREE_POINTS});

  static_cast<void>(steer(FIELD_BEHAVIOUR, body));

  const SteadyMotion* const motion = components().get<SteadyMotion>(body);
  ASSERT_NE(motion, nullptr);
  EXPECT_EQ(motion->topSpeed, WRITTEN_TOP_SPEED);
}

/**
 * Given a script that makes its body worth 700 points
 * When a body worth nothing steers with it
 * Then destroying it earns 700 points
 */
TEST_F(LuauScriptedBodyFixture, AScriptWritesTheScoreOfItsBody) {
  load(SCORE_WRITING_SCRIPT);
  const Entity body = addBody();
  components().add(body, ScoreValue{.points = 0});

  static_cast<void>(steer(FIELD_BEHAVIOUR, body));

  const ScoreValue* const score = components().get<ScoreValue>(body);
  ASSERT_NE(score, nullptr);
  EXPECT_EQ(score->points, WRITTEN_SCORE);
}

/**
 * Given a script that makes the gun of its body fire every 12 ticks
 * When a body with a gun steers with it
 * Then its gun waits 12 ticks between two shots
 */
TEST_F(LuauScriptedBodyFixture, AScriptWritesTheGunIntervalOfItsBody) {
  load(GUN_INTERVAL_WRITING_SCRIPT);
  const Entity body = addBody();
  components().add(body, Gun{.intervalTicks = 1});

  static_cast<void>(steer(FIELD_BEHAVIOUR, body));

  const Gun* const gun = components().get<Gun>(body);
  ASSERT_NE(gun, nullptr);
  EXPECT_EQ(gun->intervalTicks, WRITTEN_GUN_INTERVAL);
}

/**
 * Given a script that writes the gun interval of its body
 * When a body without a gun steers with it
 * Then the behaviour fails, saying the body has no such component
 */
TEST_F(LuauScriptedBodyFixture, WritingAComponentTheBodyLacksFails) {
  load(GUN_INTERVAL_WRITING_SCRIPT);
  const Entity body = addBody();

  static_cast<void>(steer(FIELD_BEHAVIOUR, body));

  EXPECT_THAT(failures().messages, ElementsAre(HasSubstr(ABSENT_GUN_FAILURE)));
}

/**
 * Given a script that sets the health of its body to 0
 * When a body with 3 points of 5 steers with it
 * Then the behaviour fails, naming the bounds of a health
 */
TEST_F(LuauScriptedBodyFixture, AHealthOfZeroIsRefused) {
  load(NO_HEALTH_WRITING_SCRIPT);
  const Entity body = addBody();
  components().add(body, THREE_OF_FIVE_POINTS);

  static_cast<void>(steer(FIELD_BEHAVIOUR, body));

  EXPECT_THAT(failures().messages, ElementsAre(HasSubstr(HEALTH_FAILURE)));
}

/**
 * Given a script that sets the health of its body to 0
 * When a body with 3 points of 5 steers with it
 * Then the body keeps its 3 points
 */
TEST_F(LuauScriptedBodyFixture, ARefusedHealthLeavesTheBodyAsItWas) {
  load(NO_HEALTH_WRITING_SCRIPT);
  const Entity body = addBody();
  components().add(body, THREE_OF_FIVE_POINTS);

  static_cast<void>(steer(FIELD_BEHAVIOUR, body));

  const Health* const health = components().get<Health>(body);
  ASSERT_NE(health, nullptr);
  EXPECT_EQ(health->points, THREE_OF_FIVE_POINTS.points);
}

/**
 * Given a script that sets the health of its body to 6
 * When a body with 3 points of 5 steers with it
 * Then the behaviour fails, naming the bounds of a health
 */
TEST_F(LuauScriptedBodyFixture, AHealthAboveTheFullHealthIsRefused) {
  load(HEALTH_ABOVE_FULL_WRITING_SCRIPT);
  const Entity body = addBody();
  components().add(body, THREE_OF_FIVE_POINTS);

  static_cast<void>(steer(FIELD_BEHAVIOUR, body));

  EXPECT_THAT(failures().messages, ElementsAre(HasSubstr(HEALTH_FAILURE)));
}

/**
 * Given a script that moves its body to an infinite x
 * When a body steers with it
 * Then the behaviour fails, saying what a position is
 */
TEST_F(LuauScriptedBodyFixture, AnInfinitePositionIsRefused) {
  load(INFINITE_POSITION_WRITING_SCRIPT);
  const Entity body = addBody();
  components().add(body, Position{.x = 0.0F, .y = 0.0F});

  static_cast<void>(steer(FIELD_BEHAVIOUR, body));

  EXPECT_THAT(failures().messages, ElementsAre(HasSubstr(POSITION_FAILURE)));
}

/**
 * Given a script that gives the box of its body a negative width
 * When a body with a box steers with it
 * Then the behaviour fails, saying what a collision box is
 */
TEST_F(LuauScriptedBodyFixture, ANegativeBoxIsRefused) {
  load(NEGATIVE_BOX_WRITING_SCRIPT);
  const Entity body = addBody();
  components().add(body, BOX_OFF_CENTER);

  static_cast<void>(steer(FIELD_BEHAVIOUR, body));

  EXPECT_THAT(failures().messages, ElementsAre(HasSubstr(BOX_FAILURE)));
}

/**
 * Given a script that makes its body worth a point and a half
 * When a body worth nothing steers with it
 * Then the behaviour fails, saying what a score is
 */
TEST_F(LuauScriptedBodyFixture, AScoreThatIsNotWholeIsRefused) {
  load(HALF_SCORE_WRITING_SCRIPT);
  const Entity body = addBody();
  components().add(body, ScoreValue{.points = 0});

  static_cast<void>(steer(FIELD_BEHAVIOUR, body));

  EXPECT_THAT(failures().messages, ElementsAre(HasSubstr(SCORE_FAILURE)));
}

/**
 * Given a script that writes the full health of its body
 * When a body with health steers with it
 * Then the behaviour fails, saying the field cannot be written
 */
TEST_F(LuauScriptedBodyFixture, TheFullHealthIsReadOnly) {
  load(FULL_HEALTH_WRITING_SCRIPT);
  const Entity body = addBody();
  components().add(body, THREE_OF_FIVE_POINTS);

  static_cast<void>(steer(FIELD_BEHAVIOUR, body));

  EXPECT_THAT(failures().messages, ElementsAre(HasSubstr(FULL_HEALTH_FAILURE)));
}

/**
 * Given a script that writes the lives of its body, a component off the list
 * When a body steers with it
 * Then the behaviour fails, saying the field cannot be written
 */
TEST_F(LuauScriptedBodyFixture, AComponentOffTheListCannotBeWritten) {
  load(LIVES_WRITING_SCRIPT);
  const Entity body = addBody();

  static_cast<void>(steer(FIELD_BEHAVIOUR, body));

  EXPECT_THAT(failures().messages, ElementsAre(HasSubstr(LIVES_FAILURE)));
}

/**
 * Given a script whose third line sets the health of its body to 0
 * When a body with health steers with it
 * Then the failure names that line
 */
TEST_F(LuauScriptedBodyFixture, ARefusedWriteNamesItsLine) {
  load(NO_HEALTH_WRITING_SCRIPT);
  const Entity body = addBody();
  components().add(body, THREE_OF_FIVE_POINTS);

  static_cast<void>(steer(FIELD_BEHAVIOUR, body));

  EXPECT_THAT(failures().messages, ElementsAre(HasSubstr(WRITING_LINE)));
}
