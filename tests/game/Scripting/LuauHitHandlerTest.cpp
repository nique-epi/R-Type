#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstddef>
#include "BehaviourScriptsTestConstants.hpp"
#include "GameException.hpp"
#include "IHitHandler.hpp"
#include "LuauBehaviourFixture.hpp"
#include "LuauHitHandlerTestConstants.hpp"
#include "LuauPreHitTestConstants.hpp"

using rtype::game::DuplicateHitHandlerException;
using rtype::game::IHitHandler;
using ::testing::ElementsAre;
using ::testing::HasSubstr;

/**
 * Given a script that declares the hit handler `echo`
 * When its declarations are handed over
 * Then a sheet can name `echo`: the handler is found under that name
 */
TEST_F(LuauBehaviourFixture, AHitHandlerIsHandedOverUnderItsName) {
  load(ECHO_SCRIPT);

  const IHitHandler* const echo = hitHandler(ECHO_HANDLER);

  ASSERT_NE(echo, nullptr);
  EXPECT_EQ(echo->name(), ECHO_HANDLER);
}

/**
 * Given the handler `echo`, which raises the fields of its hit
 * When it is told of a hit on body 21 by missile 7 that took 3 and left 2
 * Then it read id 21, missile 7, damage 3, health 2, and no destruction
 */
TEST_F(LuauBehaviourFixture, AHitHandlerReadsTheHit) {
  load(ECHO_SCRIPT);
  beginTick();

  hitHandler(ECHO_HANDLER)->onHit(HIT_THAT_LEFT_TWO);

  EXPECT_THAT(failures().messages,
              ElementsAre(HasSubstr(ECHO_OF_THE_HIT_THAT_LEFT_TWO)));
}

/**
 * Given the handler `echo`
 * When it is told of a hit on a body without Health
 * Then it read a health of nil
 */
TEST_F(LuauBehaviourFixture, AHitHandlerReadsNoHealthOfABodyWithoutHealth) {
  load(ECHO_SCRIPT);
  beginTick();

  hitHandler(ECHO_HANDLER)->onHit(HIT_ON_A_STEEL_WALL);

  EXPECT_THAT(failures().messages,
              ElementsAre(HasSubstr(ECHO_OF_THE_HIT_ON_A_STEEL_WALL)));
}

/**
 * Given the handler `crack`, which always fails
 * When it is told of two hits
 * Then its failure is reported once, with its name
 */
TEST_F(LuauBehaviourFixture, AFailingHitHandlerIsReportedOnce) {
  load(CRACKING_SCRIPT);
  beginTick();

  hitHandler(CRACK_HANDLER)->onHit(HIT_THAT_LEFT_TWO);
  hitHandler(CRACK_HANDLER)->onHit(HIT_THAT_LEFT_TWO);

  ASSERT_EQ(failures().messages.size(), REPORTED_ONCE);
  EXPECT_THAT(failures().messages[0], HasSubstr(FAILURE_OF_THE_CRACK_HANDLER));
}

/**
 * Given the handler `slow`, which waits
 * When it is told of a hit
 * Then it is refused: an on_hit function may not wait
 */
TEST_F(LuauBehaviourFixture, AHitHandlerThatWaitsIsReported) {
  load(WAITING_SCRIPT);
  beginTick();

  hitHandler(SLOW_HANDLER)->onHit(HIT_THAT_LEFT_TWO);

  EXPECT_THAT(failures().messages, ElementsAre(HasSubstr(WAIT_REFUSAL)));
}

/**
 * Given the handler `crack`, which always fails, and steps that spend the
 * whole budget of the tick
 * When it is told of a hit
 * Then it is not called: nothing is reported
 */
TEST_F(LuauBehaviourFixture, AHitHandlerPastTheBudgetOfTheTickIsNotCalled) {
  load(CRACKING_SCRIPT);
  beginTick();
  for (std::size_t step = 0; step < STEPS_THAT_SPEND_A_TICK; ++step) {
    SteeredBody stuck{.position = BODY_IN_THE_MIDDLE};
    static_cast<void>(steer(STUCK_BEHAVIOUR, stuck));
  }
  const std::size_t reportedByTheSteps = failures().messages.size();

  hitHandler(CRACK_HANDLER)->onHit(HIT_THAT_LEFT_TWO);

  EXPECT_EQ(failures().messages.size(), reportedByTheSteps);
}

/**
 * Given a script that declares two hit handlers named `twin`
 * When its declarations are handed over
 * Then they are refused: a sheet could not tell them apart
 */
TEST_F(LuauBehaviourFixture, RefusesTwoHitHandlersOfOneName) {
  EXPECT_THROW(load(TWIN_SCRIPT), DuplicateHitHandlerException);
}
