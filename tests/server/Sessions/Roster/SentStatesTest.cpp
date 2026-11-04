#include <gtest/gtest.h>
#include <cstdint>
#include "ProtocolConstants.hpp"
#include "SentState.hpp"
#include "SentStates.hpp"
#include "SentStatesConstants.hpp"
#include "SentStatesFixtures.hpp"
#include "SentStatesTestConstants.hpp"

namespace rtype::server {

using testing::acknowledging;
using testing::FIRST_PART_SEQUENCE;
using testing::FIRST_SENT_TICK;
using testing::FIRST_SEQUENCE_AFTER_WRAPPING;
using testing::LAST_SEQUENCE_BEFORE_WRAPPING;
using testing::NO_EARLIER_DATAGRAM;
using testing::ONE_TICK_BEYOND_THE_REACH;
using testing::ONLY_THE_DATAGRAM_JUST_BEFORE;
using testing::SECOND_PART_SEQUENCE;
using testing::SECOND_SENT_TICK;
using testing::sentStateOf;
using testing::THIRD_SENT_TICK;
using testing::THIRD_STATE_SEQUENCE;
using testing::TICK_OF_A_NEW_GAME;
using testing::TICK_OF_A_RUN_START;

/**
 * Given states sent and nothing acknowledged
 * When a reference is asked for
 * Then there is none
 */
TEST(SentStates, HasNoReferenceBeforeAnyAcknowledgement) {
  SentStates states;
  states.remember(sentStateOf(FIRST_SENT_TICK, {FIRST_PART_SEQUENCE}));

  EXPECT_EQ(states.referenceFor(SECOND_SENT_TICK), nullptr);
}

/**
 * Given a state of two parts and a datagram that acknowledges both
 * When a reference is asked for the next tick
 * Then it is that state
 */
TEST(SentStates, TakesAStateWhoseEveryPartIsAcknowledgedAsTheReference) {
  SentStates states;
  states.remember(sentStateOf(FIRST_SENT_TICK,
                              {FIRST_PART_SEQUENCE, SECOND_PART_SEQUENCE}));

  states.acknowledge(
      acknowledging(SECOND_PART_SEQUENCE, ONLY_THE_DATAGRAM_JUST_BEFORE));

  const SentState* const reference = states.referenceFor(SECOND_SENT_TICK);
  ASSERT_NE(reference, nullptr);
  EXPECT_EQ(reference->tick, FIRST_SENT_TICK);
}

/**
 * Given a state of two parts and a datagram that acknowledges the second only
 * When a reference is asked for the next tick
 * Then there is none, since the first part may be lost
 */
TEST(SentStates, DoesNotTakeAStateWithAPartNotAcknowledged) {
  SentStates states;
  states.remember(sentStateOf(FIRST_SENT_TICK,
                              {FIRST_PART_SEQUENCE, SECOND_PART_SEQUENCE}));

  states.acknowledge(acknowledging(SECOND_PART_SEQUENCE, NO_EARLIER_DATAGRAM));

  EXPECT_EQ(states.referenceFor(SECOND_SENT_TICK), nullptr);
}

/**
 * Given a state with no part, as if nothing had been sent
 * When any datagram is acknowledged
 * Then the state is not a reference
 */
TEST(SentStates, NeverAcknowledgesAStateWithoutParts) {
  SentStates states;
  states.remember(sentStateOf(FIRST_SENT_TICK, {}));

  states.acknowledge(acknowledging(FIRST_PART_SEQUENCE, NO_EARLIER_DATAGRAM));

  EXPECT_EQ(states.referenceFor(SECOND_SENT_TICK), nullptr);
}

/**
 * Given three states of which the second and the first are acknowledged
 * When a reference is asked for after the third
 * Then it is the second, the newest acknowledged
 */
TEST(SentStates, TakesTheNewestAcknowledgedState) {
  SentStates states;
  states.remember(sentStateOf(FIRST_SENT_TICK, {FIRST_PART_SEQUENCE}));
  states.remember(sentStateOf(SECOND_SENT_TICK, {SECOND_PART_SEQUENCE}));
  states.remember(sentStateOf(THIRD_SENT_TICK, {THIRD_STATE_SEQUENCE}));

  states.acknowledge(
      acknowledging(SECOND_PART_SEQUENCE, ONLY_THE_DATAGRAM_JUST_BEFORE));

  const SentState* const reference = states.referenceFor(THIRD_SENT_TICK + 1);
  ASSERT_NE(reference, nullptr);
  EXPECT_EQ(reference->tick, SECOND_SENT_TICK);
}

/**
 * Given the second state acknowledged
 * When a later datagram acknowledges only the first
 * Then the reference stays the second
 */
TEST(SentStates, NeverGoesBackToAnOlderAcknowledgement) {
  SentStates states;
  states.remember(sentStateOf(FIRST_SENT_TICK, {FIRST_PART_SEQUENCE}));
  states.remember(sentStateOf(SECOND_SENT_TICK, {SECOND_PART_SEQUENCE}));
  states.acknowledge(acknowledging(SECOND_PART_SEQUENCE, NO_EARLIER_DATAGRAM));

  states.acknowledge(acknowledging(FIRST_PART_SEQUENCE, NO_EARLIER_DATAGRAM));

  const SentState* const reference = states.referenceFor(THIRD_SENT_TICK);
  ASSERT_NE(reference, nullptr);
  EXPECT_EQ(reference->tick, SECOND_SENT_TICK);
}

/**
 * Given an acknowledged state
 * When the reference is asked for the tick as far ahead as a delta reaches,
 * then one tick further
 * Then it is given for the first and refused for the second
 */
TEST(SentStates, RefusesAReferenceBeyondTheReachOfADelta) {
  SentStates states;
  states.remember(sentStateOf(FIRST_SENT_TICK, {FIRST_PART_SEQUENCE}));
  states.acknowledge(acknowledging(FIRST_PART_SEQUENCE, NO_EARLIER_DATAGRAM));
  const std::uint32_t farthest =
      FIRST_SENT_TICK + network::MAXIMUM_REFERENCE_DISTANCE;

  EXPECT_NE(states.referenceFor(farthest), nullptr);
  EXPECT_EQ(states.referenceFor(farthest + ONE_TICK_BEYOND_THE_REACH), nullptr);
}

/**
 * Given an acknowledged state
 * When the reference is asked for the tick of that state itself
 * Then there is none: a state is never written against itself
 */
TEST(SentStates, NeverGivesAStateAsItsOwnReference) {
  SentStates states;
  states.remember(sentStateOf(FIRST_SENT_TICK, {FIRST_PART_SEQUENCE}));
  states.acknowledge(acknowledging(FIRST_PART_SEQUENCE, NO_EARLIER_DATAGRAM));

  EXPECT_EQ(states.referenceFor(FIRST_SENT_TICK), nullptr);
}

/**
 * Given a state whose parts went in the datagrams 65535 and 0
 * When a datagram acknowledges 0 and, one before it, 65535
 * Then the state is acknowledged across the wrap of the sequence
 */
TEST(SentStates, AcknowledgesAStateWhosePartsWrapTheSequence) {
  SentStates states;
  states.remember(sentStateOf(
      FIRST_SENT_TICK,
      {LAST_SEQUENCE_BEFORE_WRAPPING, FIRST_SEQUENCE_AFTER_WRAPPING}));

  states.acknowledge(acknowledging(FIRST_SEQUENCE_AFTER_WRAPPING,
                                   ONLY_THE_DATAGRAM_JUST_BEFORE));

  EXPECT_NE(states.referenceFor(SECOND_SENT_TICK), nullptr);
}

/**
 * Given an acknowledged state, then a state of a lower tick, as when a new
 * game starts
 * When a reference is asked for
 * Then the acknowledgement of the old game is forgotten
 */
TEST(SentStates, ForgetsTheAcknowledgementWhenTheTickGoesBack) {
  SentStates states;
  states.remember(sentStateOf(TICK_OF_A_RUN_START, {FIRST_PART_SEQUENCE}));
  states.acknowledge(acknowledging(FIRST_PART_SEQUENCE, NO_EARLIER_DATAGRAM));

  states.remember(sentStateOf(TICK_OF_A_NEW_GAME, {SECOND_PART_SEQUENCE}));

  EXPECT_EQ(states.referenceFor(TICK_OF_A_NEW_GAME + 1), nullptr);
}

/**
 * Given more states sent than the history keeps, the acknowledged one being
 * the first, which the history dropped
 * When a reference is asked for
 * Then there is none
 */
TEST(SentStates, DropsTheOldestStateBeyondTheHistory) {
  SentStates states;
  states.remember(sentStateOf(FIRST_SENT_TICK, {FIRST_PART_SEQUENCE}));
  states.acknowledge(acknowledging(FIRST_PART_SEQUENCE, NO_EARLIER_DATAGRAM));
  for (std::uint32_t added = 1; added <= SENT_STATE_HISTORY_SIZE; ++added) {
    states.remember(
        sentStateOf(FIRST_SENT_TICK + added, {THIRD_STATE_SEQUENCE}));
  }

  EXPECT_EQ(states.referenceFor(FIRST_SENT_TICK + SENT_STATE_HISTORY_SIZE),
            nullptr);
}

}  // namespace rtype::server
