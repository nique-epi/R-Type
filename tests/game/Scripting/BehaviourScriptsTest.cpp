#include <gtest/gtest.h>
#include "BehaviourRegistry.hpp"
#include "BehaviourScripts.hpp"
#include "BehaviourScriptsFixtures.hpp"
#include "BehaviourScriptsTestConstants.hpp"
#include "CollisionBox.hpp"
#include "EnemyPhase.hpp"
#include "EngineException.hpp"
#include "GameException.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"

using rtype::engine::ScriptException;
using rtype::game::BehaviourRegistry;
using rtype::game::BehaviourScripts;
using rtype::game::DuplicateBehaviourException;
using rtype::game::EnemyPhase;
using rtype::game::IBehaviour;
using rtype::game::Intent;

/**
 * Given a script that registers `charge` with one setting
 * When the behaviours are handed over
 * Then `charge` is among them, with its setting and its range
 */
TEST(BehaviourScripts, RegistersABehaviourAndItsSettings) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  scripts.run(scriptsOf(CHARGE_SCRIPT));

  const BehaviourRegistry behaviours = scripts.behaviours();

  const IBehaviour* const charge = behaviours.find(CHARGE_BEHAVIOUR);
  ASSERT_NE(charge, nullptr);
  ASSERT_EQ(charge->settings().size(), SETTINGS_OF_CHARGE);
  EXPECT_EQ(charge->settings()[0].name, SPEED_SHARE_SETTING);
  EXPECT_EQ(charge->settings()[0].range.lowest, LOWEST_SPEED_SHARE);
  EXPECT_EQ(charge->settings()[0].range.highest, HIGHEST_SPEED_SHARE);
}

/**
 * Given `charge` at half its speed and a ship below it
 * When it steers
 * Then it goes left at half its speed and down at its full speed
 */
TEST(BehaviourScripts, SteersAsTheScriptReadsTheView) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  scripts.run(scriptsOf(CHARGE_SCRIPT));
  const BehaviourRegistry behaviours = scripts.behaviours();

  const Intent intent =
      steeringOf(behaviours, CHARGE_BEHAVIOUR,
                 woundedViewAt(BODY_IN_THE_MIDDLE, SHIP_BELOW));

  EXPECT_EQ(intent.direction.x, HALF_LEFTWARD);
  EXPECT_EQ(intent.direction.y, FULL_DOWNWARD);
}

/**
 * Given `charge`, wounded
 * When it steers
 * Then it fires and starts the strike of its attack 1
 */
TEST(BehaviourScripts, FiresAndStartsTheActionTheScriptReturns) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  scripts.run(scriptsOf(CHARGE_SCRIPT));
  const BehaviourRegistry behaviours = scripts.behaviours();

  const Intent intent =
      steeringOf(behaviours, CHARGE_BEHAVIOUR,
                 woundedViewAt(BODY_IN_THE_MIDDLE, SHIP_BELOW));

  EXPECT_TRUE(intent.shooting);
  ASSERT_TRUE(intent.startsAction.has_value());
  EXPECT_EQ(intent.startsAction.value_or(rtype::game::ActionKey{}).attack,
            CHARGE_ATTACK);
  EXPECT_EQ(intent.startsAction.value_or(rtype::game::ActionKey{}).phase,
            EnemyPhase::Strike);
}

/**
 * Given a behaviour that asks five times its full speed
 * When it steers
 * Then its direction is cut to its full speed on each axis
 */
TEST(BehaviourScripts, CutsADirectionToTheFullSpeed) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  scripts.run(scriptsOf(TOO_FAST_SCRIPT));
  const BehaviourRegistry behaviours = scripts.behaviours();

  const Intent intent =
      steeringOf(behaviours, TOO_FAST_BEHAVIOUR,
                 woundedViewAt(BODY_IN_THE_MIDDLE, SHIP_BELOW));

  EXPECT_EQ(intent.direction.x, FULL_LEFTWARD);
  EXPECT_EQ(intent.direction.y, FULL_DOWNWARD);
}

/**
 * Given a behaviour whose script fails
 * When it steers twice
 * Then the body stays still and the failure is reported once
 */
TEST(BehaviourScripts, LeavesTheBodyStillAndReportsAFailureOnce) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  scripts.run(scriptsOf(FAILING_SCRIPT));
  const BehaviourRegistry behaviours = scripts.behaviours();
  static_cast<void>(steeringOf(behaviours, FAILING_BEHAVIOUR,
                               woundedViewAt(BODY_IN_THE_MIDDLE, SHIP_BELOW)));

  const Intent intent =
      steeringOf(behaviours, FAILING_BEHAVIOUR,
                 woundedViewAt(BODY_IN_THE_MIDDLE, SHIP_BELOW));

  EXPECT_EQ(intent.direction.x, 0.0F);
  EXPECT_EQ(intent.direction.y, 0.0F);
  EXPECT_EQ(log.messages.size(), REPORTED_ONCE);
}

/**
 * Given a script that registers a behaviour under the name of a built-in one
 * When the behaviours are handed over
 * Then it is refused
 */
TEST(BehaviourScripts, RefusesTheNameOfABuiltInBehaviour) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  scripts.run(scriptsOf(STEALS_A_BUILT_IN_NAME));

  EXPECT_THROW(static_cast<void>(scripts.behaviours()),
               DuplicateBehaviourException);
}

/**
 * Given a script that registers a behaviour without its function
 * When it runs
 * Then it fails
 */
TEST(BehaviourScripts, RefusesARegistrationWithoutAFunction) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THROW(scripts.run(scriptsOf(REGISTERS_WITHOUT_FUNCTION)),
               ScriptException);
}

/**
 * Given a behaviour that, under half its health, asks for a larger box
 * When it steers with a quarter of its health
 * Then it asks for that box
 */
TEST(BehaviourScripts, ResizesTheBodyAsTheScriptAsks) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  scripts.run(scriptsOf(ENRAGING_SCRIPT));
  const BehaviourRegistry behaviours = scripts.behaviours();

  const Intent intent = steeringOf(behaviours, ENRAGING_BEHAVIOUR,
                                   viewWithHealth(A_QUARTER_OF_ITS_HEALTH));

  ASSERT_TRUE(intent.resizesTo.has_value());
  EXPECT_EQ(intent.resizesTo.value_or(rtype::game::CollisionBox{}).width,
            ENRAGED_WIDTH);
  EXPECT_EQ(intent.resizesTo.value_or(rtype::game::CollisionBox{}).height,
            ENRAGED_HEIGHT);
}

/**
 * Given a behaviour that, under half its health, asks for twice its speed
 * When it steers with a quarter of its health
 * Then it asks for that speed
 */
TEST(BehaviourScripts, ChangesTheTopSpeedAsTheScriptAsks) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  scripts.run(scriptsOf(ENRAGING_SCRIPT));
  const BehaviourRegistry behaviours = scripts.behaviours();

  const Intent intent = steeringOf(behaviours, ENRAGING_BEHAVIOUR,
                                   viewWithHealth(A_QUARTER_OF_ITS_HEALTH));

  EXPECT_EQ(intent.topSpeedTo.value_or(0.0F), ENRAGED_TOP_SPEED);
}

/**
 * Given the same behaviour with all its health
 * When it steers
 * Then it asks for no change of its body
 */
TEST(BehaviourScripts, AsksNoChangeOfTheBodyWhenTheScriptReturnsNone) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  scripts.run(scriptsOf(ENRAGING_SCRIPT));
  const BehaviourRegistry behaviours = scripts.behaviours();

  const Intent intent = steeringOf(behaviours, ENRAGING_BEHAVIOUR,
                                   viewWithHealth(ALL_ITS_HEALTH));

  EXPECT_FALSE(intent.resizesTo.has_value());
  EXPECT_FALSE(intent.topSpeedTo.has_value());
}
