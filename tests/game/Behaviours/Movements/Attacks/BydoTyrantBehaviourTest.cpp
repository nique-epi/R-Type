#include <gtest/gtest.h>
#include <optional>
#include "BehaviourView.hpp"
#include "BydoTyrantBehaviour.hpp"
#include "BydoTyrantBehaviourFixtures.hpp"
#include "BydoTyrantBehaviourTestConstants.hpp"
#include "Intent.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::game::BehaviourView;
using rtype::game::BydoTyrantBehaviour;
using rtype::game::Intent;
using rtype::game::Position;
using rtype::game::testing::A_QUARTER_INTO_THE_CHARGE;
using rtype::game::testing::A_QUARTER_INTO_THE_CHARGE_X;
using rtype::game::testing::ARRIVAL_SIGNAL;
using rtype::game::testing::ARRIVAL_STRIKE;
using rtype::game::testing::BURST_SIGNAL;
using rtype::game::testing::BURST_STRIKE;
using rtype::game::testing::CHARGE_STRIKE;
using rtype::game::testing::ENRAGED_BURST_SIGNAL;
using rtype::game::testing::ENRAGED_BURST_SIGNAL_LENGTH;
using rtype::game::testing::ENRAGED_BURST_STRIKE;
using rtype::game::testing::ENRAGED_TENTACLE_SIGNAL;
using rtype::game::testing::HALFWAY_THROUGH_THE_SLIDE;
using rtype::game::testing::HALFWAY_THROUGH_THE_SLIDE_X;
using rtype::game::testing::INTO_ITS_RAGE;
using rtype::game::testing::JUST_SHORT_OF_THE_ENRAGED_BURST_SIGNAL;
using rtype::game::testing::PAST_THE_TURN_OF_THE_CHARGE;
using rtype::game::testing::RAGE;
using rtype::game::testing::SHIP_FAR_FROM_THE_TYRANT;
using rtype::game::testing::SHIP_UNDER_ITS_JAW;
using rtype::game::testing::SWEEP_SIGNAL;
using rtype::game::testing::TENTACLE_SIGNAL;
using rtype::game::testing::TWO_ENDED_ACTIONS;
using rtype::game::testing::TYRANT_AT_REST;
using rtype::game::testing::TYRANT_COLUMN_TOLERANCE;
using rtype::game::testing::TYRANT_PAST_THE_LEFT_EDGE;
using rtype::game::testing::TYRANT_REACH_IN_A_TICK;
using rtype::game::testing::TYRANT_SHORT_OF_ITS_REST;
using rtype::game::testing::tyrantView;
using rtype::game::testing::UNKNOWN_TYRANT_ATTACK;
using rtype::game::testing::WHERE_THE_CHARGE_COMES_BACK;
using rtype::game::testing::WHERE_THE_TYRANT_APPEARS;
using rtype::game::testing::WHOLE_RAGE;
using rtype::game::testing::WHOLE_SLIDE;

/**
 * Given a Bydo Tyrant on its first tick, which has ended no action yet
 * When it steers
 * Then it starts the signal of its arrival
 */
TEST(BydoTyrantBehaviour, ItStartsItsArrivalOnItsFirstTick) {
  BehaviourView view =
      tyrantView(WHERE_THE_TYRANT_APPEARS, SHIP_FAR_FROM_THE_TYRANT);
  view.age = SIMULATION_TICK_DURATION;
  view.actionAge = SIMULATION_TICK_DURATION;
  view.actionsEnded = 0;

  EXPECT_EQ(BydoTyrantBehaviour{}.steer(view).startsAction, ARRIVAL_SIGNAL);
}

/**
 * Given a Tyrant halfway through the 1800 ms slide of its arrival, still where
 * it appeared, at column 2176
 * When it steers
 * Then it heads for column 1896, halfway to the column it rests at
 */
TEST(BydoTyrantBehaviour, ItSlidesInDuringItsArrival) {
  BehaviourView view =
      tyrantView(WHERE_THE_TYRANT_APPEARS, SHIP_FAR_FROM_THE_TYRANT);
  view.age = HALFWAY_THROUGH_THE_SLIDE;
  view.action = ARRIVAL_SIGNAL;
  view.actionAge = HALFWAY_THROUGH_THE_SLIDE;
  view.actionsEnded = 0;

  const Intent intent = BydoTyrantBehaviour{}.steer(view);

  EXPECT_NEAR(view.position.x + (intent.direction.x * TYRANT_REACH_IN_A_TICK),
              HALFWAY_THROUGH_THE_SLIDE_X, TYRANT_COLUMN_TOLERANCE);
  EXPECT_FALSE(intent.startsAction.has_value());
}

/**
 * Given a Tyrant whose slide has lasted its 1800 ms
 * When it steers
 * Then it starts the strike of its arrival: its roar
 */
TEST(BydoTyrantBehaviour, ItRoarsOnceItsSlideIsOver) {
  BehaviourView view = tyrantView(TYRANT_AT_REST, SHIP_FAR_FROM_THE_TYRANT);
  view.age = WHOLE_SLIDE;
  view.action = ARRIVAL_SIGNAL;
  view.actionAge = WHOLE_SLIDE;
  view.actionsEnded = 0;

  EXPECT_EQ(BydoTyrantBehaviour{}.steer(view).startsAction, ARRIVAL_STRIKE);
}

/**
 * Given a calm Tyrant whose arrival ended and that has rested, a ship far
 * away
 * When it steers
 * Then it starts the signal of its burst, the first attack after its arrival
 */
TEST(BydoTyrantBehaviour, ItOpensWithACalmBurstAfterItsArrival) {
  const BehaviourView view =
      tyrantView(TYRANT_AT_REST, SHIP_FAR_FROM_THE_TYRANT);

  EXPECT_EQ(BydoTyrantBehaviour{}.steer(view).startsAction, BURST_SIGNAL);
}

/**
 * Given a calm Tyrant idle for 500 ms of its 1000 ms rest
 * When it steers
 * Then it starts no action
 */
TEST(BydoTyrantBehaviour, ItRestsBeforeItsNextAttack) {
  BehaviourView view = tyrantView(TYRANT_AT_REST, SHIP_FAR_FROM_THE_TYRANT);
  view.actionAge = TYRANT_SHORT_OF_ITS_REST;

  EXPECT_FALSE(BydoTyrantBehaviour{}.steer(view).startsAction.has_value());
}

/**
 * Given a calm, rested Tyrant at rest and a ship on its melee point
 * When it steers
 * Then it starts the signal of its tentacles, not the next attack
 */
TEST(BydoTyrantBehaviour, ItLashesItsTentaclesAtAShipUnderItsJaw) {
  const BehaviourView view = tyrantView(TYRANT_AT_REST, SHIP_UNDER_ITS_JAW);

  EXPECT_EQ(BydoTyrantBehaviour{}.steer(view).startsAction, TENTACLE_SIGNAL);
}

/**
 * Given an enraged, rested Tyrant and a ship on its melee point
 * When it steers
 * Then it starts the signal of its enraged tentacles
 */
TEST(BydoTyrantBehaviour, EnragedItLashesItsEnragedTentacles) {
  BehaviourView view = tyrantView(TYRANT_AT_REST, SHIP_UNDER_ITS_JAW);
  view.enraged = true;

  EXPECT_EQ(BydoTyrantBehaviour{}.steer(view).startsAction,
            ENRAGED_TENTACLE_SIGNAL);
}

/**
 * Given an enraged Tyrant 420 ms into the signal of its enraged burst, 70 %
 * of the 600 ms of its calm one
 * When it steers
 * Then it strikes: its enraged signals are 30 % shorter
 */
TEST(BydoTyrantBehaviour, EnragedItSignalsThirtyPercentShorter) {
  BehaviourView view = tyrantView(TYRANT_AT_REST, SHIP_FAR_FROM_THE_TYRANT);
  view.enraged = true;
  view.action = ENRAGED_BURST_SIGNAL;
  view.actionAge = ENRAGED_BURST_SIGNAL_LENGTH;

  EXPECT_EQ(BydoTyrantBehaviour{}.steer(view).startsAction,
            ENRAGED_BURST_STRIKE);
}

/**
 * Given an enraged Tyrant 419 ms into the signal of its enraged burst
 * When it steers
 * Then it starts nothing: its shortened signal lasts 420 ms
 */
TEST(BydoTyrantBehaviour, ItDoesNotStrikeBeforeItsShortenedSignal) {
  BehaviourView view = tyrantView(TYRANT_AT_REST, SHIP_FAR_FROM_THE_TYRANT);
  view.enraged = true;
  view.action = ENRAGED_BURST_SIGNAL;
  view.actionAge = JUST_SHORT_OF_THE_ENRAGED_BURST_SIGNAL;

  EXPECT_FALSE(BydoTyrantBehaviour{}.steer(view).startsAction.has_value());
}

/**
 * Given an enraged, rested Tyrant that has ended two actions
 * When it steers
 * Then it starts the signal of its sweeping beam, the third attack of its
 * enraged rotation
 */
TEST(BydoTyrantBehaviour, EnragedItTakesItsSecondRotation) {
  BehaviourView view = tyrantView(TYRANT_AT_REST, SHIP_FAR_FROM_THE_TYRANT);
  view.enraged = true;
  view.actionsEnded = TWO_ENDED_ACTIONS;

  EXPECT_EQ(BydoTyrantBehaviour{}.steer(view).startsAction, SWEEP_SIGNAL);
}

/**
 * Given a Tyrant 300 ms into its rage
 * When it steers
 * Then it holds still and does not end its rage
 */
TEST(BydoTyrantBehaviour, ItHoldsStillInItsRage) {
  BehaviourView view = tyrantView(TYRANT_AT_REST, SHIP_FAR_FROM_THE_TYRANT);
  view.enraged = true;
  view.action = RAGE;
  view.actionAge = INTO_ITS_RAGE;

  const Intent intent = BydoTyrantBehaviour{}.steer(view);

  EXPECT_FLOAT_EQ(intent.direction.x, 0.0F);
  EXPECT_FLOAT_EQ(intent.direction.y, 0.0F);
  EXPECT_FALSE(intent.endsAction);
}

/**
 * Given a Tyrant whose rage has lasted its 700 ms
 * When it steers
 * Then it ends its rage
 */
TEST(BydoTyrantBehaviour, ItEndsItsRageAfterRageMs) {
  BehaviourView view = tyrantView(TYRANT_AT_REST, SHIP_FAR_FROM_THE_TYRANT);
  view.enraged = true;
  view.action = RAGE;
  view.actionAge = WHOLE_RAGE;

  EXPECT_TRUE(BydoTyrantBehaviour{}.steer(view).endsAction);
}

/**
 * Given a Tyrant at rest a quarter into the 1600 ms strike of its charge
 * When it steers
 * Then it heads for column 648, halfway out to past the left edge
 */
TEST(BydoTyrantBehaviour, ItLeavesOnTheLeftInTheFirstHalfOfItsCharge) {
  BehaviourView view = tyrantView(TYRANT_AT_REST, SHIP_FAR_FROM_THE_TYRANT);
  view.enraged = true;
  view.action = CHARGE_STRIKE;
  view.actionAge = A_QUARTER_INTO_THE_CHARGE;

  const Intent intent = BydoTyrantBehaviour{}.steer(view);

  EXPECT_NEAR(view.position.x + (intent.direction.x * TYRANT_REACH_IN_A_TICK),
              A_QUARTER_INTO_THE_CHARGE_X, TYRANT_COLUMN_TOLERANCE);
  EXPECT_FALSE(intent.jumpsTo.has_value());
}

/**
 * Given a Tyrant past the left edge 1000 ms into the strike of its charge,
 * past its turn
 * When it steers
 * Then it leaps to column 2084, coming back from past the right edge
 */
TEST(BydoTyrantBehaviour, ItComesBackFromTheRightPastTheTurnOfItsCharge) {
  BehaviourView view =
      tyrantView(TYRANT_PAST_THE_LEFT_EDGE, SHIP_FAR_FROM_THE_TYRANT);
  view.enraged = true;
  view.action = CHARGE_STRIKE;
  view.actionAge = PAST_THE_TURN_OF_THE_CHARGE;

  const Intent intent = BydoTyrantBehaviour{}.steer(view);

  ASSERT_TRUE(intent.jumpsTo.has_value());
  const Position leap = intent.jumpsTo.value_or(Position{});
  EXPECT_FLOAT_EQ(leap.x, WHERE_THE_CHARGE_COMES_BACK.x);
  EXPECT_FLOAT_EQ(leap.y, WHERE_THE_CHARGE_COMES_BACK.y);
}

/**
 * Given a Tyrant in the strike of its burst
 * When it steers
 * Then it does not shoot: its missiles are the volleys of its sheet
 */
TEST(BydoTyrantBehaviour, ItNeverShoots) {
  BehaviourView view = tyrantView(TYRANT_AT_REST, SHIP_FAR_FROM_THE_TYRANT);
  view.action = BURST_STRIKE;
  view.actionAge = SIMULATION_TICK_DURATION;

  EXPECT_FALSE(BydoTyrantBehaviour{}.steer(view).shooting);
}

/**
 * Given a Tyrant in an attack it does not know
 * When it steers
 * Then it ends that action
 */
TEST(BydoTyrantBehaviour, ItEndsAnActionItDoesNotKnow) {
  BehaviourView view = tyrantView(TYRANT_AT_REST, SHIP_FAR_FROM_THE_TYRANT);
  view.action = UNKNOWN_TYRANT_ATTACK;

  EXPECT_TRUE(BydoTyrantBehaviour{}.steer(view).endsAction);
}
