#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <string_view>
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
 * Given `sparkle`, which names the effect spark and the look cracked
 * When it steers
 * Then its intent plays spark and shows cracked
 */
TEST(BehaviourScripts, KeepsTheEffectAndTheLookTheScriptNames) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  scripts.run(scriptsOf(VISUALS_SCRIPT));
  const BehaviourRegistry behaviours = scripts.behaviours();

  const Intent intent =
      steeringOf(behaviours, SPARKLE_BEHAVIOUR,
                 woundedViewAt(BODY_IN_THE_MIDDLE, SHIP_BELOW));

  EXPECT_EQ(intent.playsEffect, std::optional{std::string{SPARK_EFFECT}});
  EXPECT_EQ(intent.showsLook, std::optional{std::string{CRACKED_LOOK}});
}

/**
 * Given `shout`, which names an effect with a capital letter and a look with a
 * space, and `numbers`, which gives numbers instead of names
 * When each steers
 * Then neither intent plays an effect or shows a look
 */
TEST(BehaviourScripts, DropsAnEffectOrALookThatIsNoNameOfTheContent) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  scripts.run(scriptsOf(VISUALS_SCRIPT));
  const BehaviourRegistry behaviours = scripts.behaviours();

  for (const std::string_view name : {SHOUT_BEHAVIOUR, NUMBERS_BEHAVIOUR}) {
    SCOPED_TRACE(name);
    const Intent intent = steeringOf(
        behaviours, name, woundedViewAt(BODY_IN_THE_MIDDLE, SHIP_BELOW));
    EXPECT_FALSE(intent.playsEffect.has_value());
    EXPECT_FALSE(intent.showsLook.has_value());
  }
}

/**
 * Given `plain`, which asks for the empty look
 * When it steers
 * Then its intent shows the empty look, the idle loop of its sheet
 */
TEST(BehaviourScripts, KeepsTheEmptyLook) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  scripts.run(scriptsOf(VISUALS_SCRIPT));
  const BehaviourRegistry behaviours = scripts.behaviours();

  const Intent intent =
      steeringOf(behaviours, PLAIN_BEHAVIOUR,
                 woundedViewAt(BODY_IN_THE_MIDDLE, SHIP_BELOW));

  EXPECT_EQ(intent.showsLook, std::optional{std::string{IDLE_LOOK}});
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
 * Given a behaviour that fires while its boss enraged
 * When an enraged boss steers with it
 * Then it fires
 */
TEST(BehaviourScripts, ShowsTheScriptThatItsBossEnraged) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  scripts.run(scriptsOf(BOSS_READING_SCRIPT));
  const BehaviourRegistry behaviours = scripts.behaviours();

  const Intent intent =
      steeringOf(behaviours, BOSS_READING_BEHAVIOUR, viewOfABoss(true));

  EXPECT_TRUE(intent.shooting);
}

/**
 * Given the same behaviour
 * When a calm boss steers with it
 * Then it holds its fire
 */
TEST(BehaviourScripts, ShowsTheScriptThatItsBossIsCalm) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  scripts.run(scriptsOf(BOSS_READING_SCRIPT));
  const BehaviourRegistry behaviours = scripts.behaviours();

  const Intent intent =
      steeringOf(behaviours, BOSS_READING_BEHAVIOUR, viewOfABoss(false));

  EXPECT_FALSE(intent.shooting);
}

/**
 * Given a behaviour that takes as top speed the actions it ended
 * When a body that ended three actions steers with it
 * Then it asks for a top speed of three
 */
TEST(BehaviourScripts, ShowsTheScriptTheActionsItEnded) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  scripts.run(scriptsOf(BOSS_READING_SCRIPT));
  const BehaviourRegistry behaviours = scripts.behaviours();

  const Intent intent =
      steeringOf(behaviours, BOSS_READING_BEHAVIOUR, viewOfABoss(false));

  EXPECT_EQ(intent.topSpeedTo.value_or(0.0F), THREE_UNITS_PER_SECOND);
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

/**
 * Given the Crimson Drifter of the example resource and a ship level with it
 * When it steers
 * Then it charges at its charge speed
 */
TEST(BehaviourScripts, TheExampleDrifterChargesAShipLevelWithIt) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  const BehaviourRegistry behaviours = exampleBehaviours(scripts);

  const Intent intent =
      steeringOf(behaviours, EXAMPLE_BEHAVIOUR, exampleViewWith(SHIP_LEVEL));

  EXPECT_EQ(intent.topSpeedTo.value_or(0.0F), EXAMPLE_CHARGE_SPEED);
}

/**
 * Given the Crimson Drifter of the example resource and a ship far below it
 * When it steers
 * Then it cruises at its cruise speed
 */
TEST(BehaviourScripts, TheExampleDrifterCruisesWhenNoShipIsLevel) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  const BehaviourRegistry behaviours = exampleBehaviours(scripts);

  const Intent intent = steeringOf(behaviours, EXAMPLE_BEHAVIOUR,
                                   exampleViewWith(SHIP_FAR_BELOW));

  EXPECT_EQ(intent.topSpeedTo.value_or(0.0F), EXAMPLE_CRUISE_SPEED);
}
