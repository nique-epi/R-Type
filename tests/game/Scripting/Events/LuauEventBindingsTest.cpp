#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include "BehaviourScriptsTestConstants.hpp"
#include "EngineException.hpp"
#include "EntityDamage.hpp"
#include "EntityDestruction.hpp"
#include "Intent.hpp"
#include "LuauBehaviourFixture.hpp"
#include "LuauEventBindingsFixture.hpp"
#include "LuauEventBindingsTestConstants.hpp"
#include "LuauEventConstants.hpp"
#include "ScriptEvent.hpp"
#include "ScriptEventField.hpp"

using rtype::engine::ScriptException;
using rtype::game::DestructionCause;
using rtype::game::EntityDamage;
using rtype::game::EntityDestruction;
using rtype::game::Intent;
using rtype::game::ScriptEvent;
using rtype::game::ScriptEventField;
using ::testing::ElementsAre;
using ::testing::HasSubstr;
using ::testing::IsEmpty;

/**
 * Given a body whose script subscribed a function to the destruction of an
 * entity
 * When the game publishes the destruction of the entity 42 and dispatches
 * Then the function read the id 42 in its event
 */
TEST_F(LuauEventBindingsFixture, CallsASubscribedFunctionWithAnEventOfTheGame) {
  load(LISTENER_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  static_cast<void>(steer(LISTENER_BEHAVIOUR, body));

  bus().publish(EntityDestruction{.networkId = DESTROYED_ENTITY_ID,
                                  .cause = DestructionCause::Killed});
  bus().dispatch();

  const Intent intent = steer(LISTENER_BEHAVIOUR, body);
  EXPECT_EQ(intent.topSpeedTo.value_or(0.0F), TOP_SPEED_OF_THE_DESTROYED_ID);
}

/**
 * Given a body that subscribed to `alarm`, and another whose script publishes
 * `alarm` at the level 7
 * When the bus dispatches
 * Then the subscribed function read the level 7
 */
TEST_F(LuauEventBindingsFixture, HandsAnEventOneScriptPublishesToAnother) {
  load(ALARM_SCRIPT);
  SteeredBody listening{.position = BODY_IN_THE_MIDDLE};
  SteeredBody publishing{.position = BODY_IN_THE_MIDDLE};
  static_cast<void>(steer(ECHO_BEHAVIOUR, listening));
  static_cast<void>(steer(EMITTER_BEHAVIOUR, publishing));

  bus().dispatch();

  const Intent intent = steer(ECHO_BEHAVIOUR, listening);
  EXPECT_EQ(intent.topSpeedTo.value_or(0.0F), TOP_SPEED_OF_THE_ALARM_LEVEL);
}

/**
 * Given a body that subscribed to `alarm`
 * When an event of another name is published and dispatched
 * Then its function is not called
 */
TEST_F(LuauEventBindingsFixture, KeepsAnEventFromTheFunctionsOfAnotherName) {
  load(ALARM_SCRIPT);
  SteeredBody listening{.position = BODY_IN_THE_MIDDLE};
  static_cast<void>(steer(ECHO_BEHAVIOUR, listening));

  bus().publish(
      ScriptEvent{.name = std::string{UNHEARD_EVENT_NAME},
                  .fields = {ScriptEventField{
                      .key = LEVEL_KEY, .value = LEVEL_OF_THE_UNHEARD_EVENT}}});
  bus().dispatch();

  const Intent intent = steer(ECHO_BEHAVIOUR, listening);
  EXPECT_FALSE(intent.topSpeedTo.has_value());
}

/**
 * Given a script that publishes an event
 * When no dispatch happened yet
 * Then no subscriber heard it: it waits in the queue of the bus
 */
TEST_F(LuauEventBindingsFixture, PublishesOnlyWhenTheBusDispatches) {
  load(ALARM_SCRIPT);
  SteeredBody publishing{.position = BODY_IN_THE_MIDDLE};

  static_cast<void>(steer(EMITTER_BEHAVIOUR, publishing));

  EXPECT_THAT(published(), IsEmpty());
}

/**
 * Given two bodies that subscribed to the same event, the body 1 first
 * When the game publishes that event and dispatches
 * Then the function of the body 1 ran before the one of the body 2
 */
TEST_F(LuauEventBindingsFixture, CallsTheFunctionsInTheOrderTheySubscribed) {
  load(DAMAGE_SCRIPT);
  SteeredBody first{.position = BODY_IN_THE_MIDDLE,
                    .networkId = FIRST_RELAY_ID};
  SteeredBody second{.position = BODY_IN_THE_MIDDLE,
                     .networkId = SECOND_RELAY_ID};
  static_cast<void>(steer(RELAY_BEHAVIOUR, first));
  static_cast<void>(steer(RELAY_BEHAVIOUR, second));

  bus().publish(EntityDamage{.networkId = DAMAGED_ENTITY_ID});
  bus().dispatch();

  EXPECT_THAT(published(),
              ElementsAre(heardBy(FIRST_RELAY_ID), heardBy(SECOND_RELAY_ID)));
}

/**
 * Given a body that subscribed a function to an event
 * When the body is destroyed, then the event is published and dispatched
 * Then the function is not called
 */
TEST_F(LuauEventBindingsFixture, EndsASubscriptionWithItsEntity) {
  load(DAMAGE_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE, .networkId = FIRST_RELAY_ID};
  static_cast<void>(steer(RELAY_BEHAVIOUR, body));

  body.behaviourState.reset();
  bus().publish(EntityDamage{.networkId = DAMAGED_ENTITY_ID});
  bus().dispatch();

  EXPECT_THAT(published(), IsEmpty());
}

/**
 * Given a body whose subscribed function fails, and a body that subscribed
 * after it
 * When the event is published and dispatched
 * Then the function of the second body is still called
 */
TEST_F(LuauEventBindingsFixture, CallsTheSubscribersAfterOneThatFails) {
  load(DAMAGE_SCRIPT);
  SteeredBody failing{.position = BODY_IN_THE_MIDDLE};
  SteeredBody sound{.position = BODY_IN_THE_MIDDLE,
                    .networkId = SECOND_RELAY_ID};
  static_cast<void>(steer(FAULTY_BEHAVIOUR, failing));
  static_cast<void>(steer(RELAY_BEHAVIOUR, sound));

  bus().publish(EntityDamage{.networkId = DAMAGED_ENTITY_ID});
  bus().dispatch();

  EXPECT_THAT(published(), ElementsAre(heardBy(SECOND_RELAY_ID)));
}

/**
 * Given a body going left whose subscribed function failed
 * When it is steered at the next tick
 * Then its script is stopped: it stands still
 */
TEST_F(LuauEventBindingsFixture, StopsTheScriptOfAnEntityWhoseFunctionFails) {
  load(DAMAGE_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  ASSERT_EQ(steer(FAULTY_BEHAVIOUR, body).direction.x,
            FULL_LEFTWARD_OF_THE_FAULTY);
  bus().publish(EntityDamage{.networkId = DAMAGED_ENTITY_ID});
  bus().dispatch();

  const Intent intent = steer(FAULTY_BEHAVIOUR, body);

  EXPECT_EQ(intent.direction.x, 0.0F);
}

/**
 * Given a body whose subscribed function fails
 * When the event is dispatched and the body steered, twice
 * Then the failure is reported once, as the failure of its behaviour
 */
TEST_F(LuauEventBindingsFixture, ReportsAFailedFunctionOnce) {
  load(DAMAGE_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  static_cast<void>(steer(FAULTY_BEHAVIOUR, body));

  bus().publish(EntityDamage{.networkId = DAMAGED_ENTITY_ID});
  bus().dispatch();
  static_cast<void>(steer(FAULTY_BEHAVIOUR, body));
  bus().publish(EntityDamage{.networkId = DAMAGED_ENTITY_ID});
  bus().dispatch();
  static_cast<void>(steer(FAULTY_BEHAVIOUR, body));

  EXPECT_THAT(failures().messages,
              ElementsAre(HasSubstr(FAILED_SUBSCRIBED_FUNCTION_REPORT)));
}

/**
 * Given a body whose subscribed function waits
 * When the event is dispatched and the body steered
 * Then that is a failure, which says a subscribed function cannot wait
 */
TEST_F(LuauEventBindingsFixture, RefusesASubscribedFunctionThatWaits) {
  load(DAMAGE_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  static_cast<void>(steer(SLEEPER_BEHAVIOUR, body));

  bus().publish(EntityDamage{.networkId = DAMAGED_ENTITY_ID});
  bus().dispatch();
  static_cast<void>(steer(SLEEPER_BEHAVIOUR, body));

  EXPECT_THAT(
      failures().messages,
      ElementsAre(HasSubstr(rtype::game::WAIT_IN_SUBSCRIBED_FUNCTION_MESSAGE)));
}

/**
 * Given a script that publishes under the name of an event of the game
 * When its body steps
 * Then the step fails, saying only the game publishes its events
 */
TEST_F(LuauEventBindingsFixture, RefusesToPublishAnEventOfTheGame) {
  load(REFUSED_EMISSION_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};

  static_cast<void>(steer(FORGER_BEHAVIOUR, body));

  EXPECT_THAT(
      failures().messages,
      ElementsAre(HasSubstr(rtype::game::EMISSION_OF_GAME_EVENT_MESSAGE)));
}

/**
 * Given a script that publishes a value that is a table
 * When its body steps
 * Then the step fails, saying what the values of an event are
 */
TEST_F(LuauEventBindingsFixture, RefusesAnEventValueThatIsATable) {
  load(REFUSED_EMISSION_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};

  static_cast<void>(steer(NESTER_BEHAVIOUR, body));

  EXPECT_THAT(failures().messages,
              ElementsAre(HasSubstr(rtype::game::WRONG_EVENT_FIELD_MESSAGE)));
}

/**
 * Given a script that subscribes while it is read
 * When it is run
 * Then it is refused: only the function of a body subscribes
 */
TEST_F(LuauEventBindingsFixture, RefusesASubscriptionOutsideABody) {
  EXPECT_THROW(load(SUBSCRIPTION_AT_LOAD_SCRIPT), ScriptException);
}

/**
 * Given a body whose script subscribes a function on every tick
 * When it is steered for as many ticks as an entity may keep functions
 * Then none of them failed
 */
TEST_F(LuauEventBindingsFixture, KeepsAsManyFunctionsAsAnEntityMay) {
  load(EAGER_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};

  for (std::size_t tick = 0; tick < SUBSCRIPTIONS_AN_ENTITY_MAY_KEEP; ++tick) {
    static_cast<void>(steer(EAGER_BEHAVIOUR, body));
  }

  EXPECT_THAT(failures().messages, IsEmpty());
}

/**
 * Given a body that keeps as many functions as an entity may
 * When its script subscribes one more
 * Then the step fails, saying it subscribed too many
 */
TEST_F(LuauEventBindingsFixture, RefusesOneFunctionPastWhatAnEntityMayKeep) {
  load(EAGER_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  for (std::size_t tick = 0; tick < SUBSCRIPTIONS_AN_ENTITY_MAY_KEEP; ++tick) {
    static_cast<void>(steer(EAGER_BEHAVIOUR, body));
  }

  static_cast<void>(steer(EAGER_BEHAVIOUR, body));

  EXPECT_THAT(
      failures().messages,
      ElementsAre(HasSubstr(rtype::game::TOO_MANY_SUBSCRIPTIONS_MESSAGE)));
}

/**
 * Given two bodies whose functions answer each event of the other with one
 * When the bus dispatches
 * Then the dispatch ends, once the scripts spent the budget of the tick
 */
TEST_F(LuauEventBindingsFixture, EndsEventsThatAnswerEachOther) {
  load(PING_PONG_SCRIPT);
  SteeredBody answering{.position = BODY_IN_THE_MIDDLE};
  SteeredBody starting{.position = BODY_IN_THE_MIDDLE};
  static_cast<void>(steer(PONG_BEHAVIOUR, answering));
  static_cast<void>(steer(PING_BEHAVIOUR, starting));

  bus().dispatch();

  EXPECT_FALSE(scriptsHaveBudget());
}
