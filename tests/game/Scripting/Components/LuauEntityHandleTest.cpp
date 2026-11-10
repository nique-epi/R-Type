#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "BehaviourRegistry.hpp"
#include "BehaviourScripts.hpp"
#include "BehaviourScriptsFixtures.hpp"
#include "BehaviourScriptsTestConstants.hpp"
#include "Entity.hpp"
#include "Health.hpp"
#include "Intent.hpp"
#include "LuauEntityHandleTestConstants.hpp"
#include "LuauScriptedBodyFixture.hpp"
#include "Position.hpp"

using rtype::engine::Entity;
using rtype::game::BehaviourRegistry;
using rtype::game::BehaviourScripts;
using rtype::game::Health;
using rtype::game::Intent;
using rtype::game::Position;
using testing::ElementsAre;
using testing::HasSubstr;
using testing::IsEmpty;

/**
 * Given a script that counts the steps of its body in its data
 * When a body steers with it twice
 * Then its second step counts 2
 */
TEST_F(LuauScriptedBodyFixture, TheDataOfABodyLastsFromStepToStep) {
  load(COUNTING_SCRIPT);
  const Entity body = addBody();
  static_cast<void>(steer(HANDLE_BEHAVIOUR, body));

  const Intent intent = steer(HANDLE_BEHAVIOUR, body);

  EXPECT_EQ(intent.topSpeedTo.value_or(0.0F), AFTER_TWO_STEPS);
}

/**
 * Given a script that counts the steps of its body in its data
 * When a body steers with it twice, then another body once
 * Then the other body counts 1
 */
TEST_F(LuauScriptedBodyFixture, EachBodyHasDataOfItsOwn) {
  load(COUNTING_SCRIPT);
  const Entity first = addBody();
  const Entity second = addBody();
  static_cast<void>(steer(HANDLE_BEHAVIOUR, first));
  static_cast<void>(steer(HANDLE_BEHAVIOUR, first));

  const Intent intent = steer(HANDLE_BEHAVIOUR, second);

  EXPECT_EQ(intent.topSpeedTo.value_or(0.0F), AFTER_ONE_STEP);
}

/**
 * Given a script whose first body kept its handle in a global, then ended
 * When another body reads the data of that handle
 * Then the behaviour fails, saying the body is gone
 */
TEST_F(LuauScriptedBodyFixture, TheHandleOfAnEndedBodyIsRefused) {
  load(DATA_KEEPING_SCRIPT);
  const Entity first = addBody();
  const Entity second = addBody();
  static_cast<void>(steer(HANDLE_BEHAVIOUR, first));
  endBody(first);

  static_cast<void>(steer(HANDLE_BEHAVIOUR, second));

  EXPECT_THAT(failures().messages, ElementsAre(HasSubstr(GONE_BODY_FAILURE)));
}

/**
 * Given a script whose first body kept its handle in a global, and still lives
 * When another body reads the data of that handle
 * Then nothing fails
 */
TEST_F(LuauScriptedBodyFixture, TheDataOfALivingBodyStaysReadable) {
  load(DATA_KEEPING_SCRIPT);
  const Entity first = addBody();
  const Entity second = addBody();
  static_cast<void>(steer(HANDLE_BEHAVIOUR, first));

  static_cast<void>(steer(HANDLE_BEHAVIOUR, second));

  EXPECT_THAT(failures().messages, IsEmpty());
}

/**
 * Given a script whose first body, with health, kept its handle in a global
 * When another body reads the health of that handle
 * Then the behaviour fails, saying components are only reached in their step
 */
TEST_F(LuauScriptedBodyFixture, TheComponentsOfAnotherBodyAreRefused) {
  load(HEALTH_KEEPING_SCRIPT);
  const Entity first = addBody();
  const Entity second = addBody();
  components().add(first, Health{.points = 1, .full = 1});
  static_cast<void>(steer(HANDLE_BEHAVIOUR, first));

  static_cast<void>(steer(HANDLE_BEHAVIOUR, second));

  EXPECT_THAT(failures().messages, ElementsAre(HasSubstr(OTHER_BODY_FAILURE)));
}

/**
 * Given a script that replaces the data of its body
 * When a body steers with it
 * Then the behaviour fails, saying the data cannot be replaced
 */
TEST_F(LuauScriptedBodyFixture, TheDataOfABodyIsNeverReplaced) {
  load(DATA_REPLACING_SCRIPT);
  const Entity body = addBody();

  static_cast<void>(steer(HANDLE_BEHAVIOUR, body));

  EXPECT_THAT(failures().messages,
              ElementsAre(HasSubstr(DATA_REPLACED_FAILURE)));
}

/**
 * Given scripts attached to no game, whose body moves itself
 * When a body steers with it
 * Then the behaviour fails, saying components are only written in a game
 */
TEST(LuauEntityHandle, AWriteOutsideAGameIsRefused) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  scripts.run(scriptsOf(MOVING_SCRIPT));
  const BehaviourRegistry behaviours = scripts.behaviours();

  static_cast<void>(steeringOf(behaviours, HANDLE_BEHAVIOUR,
                               woundedViewAt(Position{}, SHIP_BELOW)));

  EXPECT_THAT(log.messages, ElementsAre(HasSubstr(OUTSIDE_GAME_FAILURE)));
}

/**
 * Given a script that fires when a wait returns the handle it received
 * When a body steers with it until its wait ends
 * Then it fires
 */
TEST_F(LuauScriptedBodyFixture, AWaitReturnsTheSameHandle) {
  load(WAITING_SCRIPT);
  const Entity body = addBody();
  static_cast<void>(steer(HANDLE_BEHAVIOUR, body));

  const Intent intent = steer(HANDLE_BEHAVIOUR, body);

  EXPECT_TRUE(intent.shooting);
}
