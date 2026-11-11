#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DeltaStateSenderFixture.hpp"
#include "DeltaStateSenderTestConstants.hpp"
#include "DeltaWorldState.hpp"
#include "Entity.hpp"
#include "EntityTypeNumberTestConstants.hpp"
#include "ProtocolConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateSenderFixture.hpp"

namespace rtype::server::testing {

using network::DeltaWorldState;
using network::EntityState;
using network::NO_REFERENCE_DISTANCE;

/**
 * Given a player who acknowledged the first state, and an enemy that moved
 * since while another stayed
 * When the next state is due
 * Then it is written against the first: the reference is the three ticks back,
 * only the enemy that moved is listed, and nothing is removed
 */
TEST_F(DeltaStateSenderFixture, SendsOnlyWhatChangedSinceTheAcknowledgedState) {
  const engine::Entity moving = addEnemy(MOVING_ENEMY_ID, FIRST_PLACE);
  addEnemy(STILL_ENEMY_ID, SECOND_PLACE);
  ConnectedPlayer& player = connect(FIRST_PLAYER_PORT);
  runTicks(TICKS_TO_THE_FIRST_STATE);
  acknowledgeSentTo(player, EVERY_EARLIER_DATAGRAM);
  move(moving, MOVED_PLACE);

  runTicks(TICKS_PER_STATE);

  const DeltaWorldState delta = partsSentTo(FIRST_PLAYER_PORT).back();
  EXPECT_EQ(delta.baseDistance, TICKS_PER_STATE);
  EXPECT_EQ(delta.changed,
            std::vector<EntityState>{listedEntity(
                MOVING_ENEMY_ID, DRIFTER_TYPE_NUMBER, MOVED_PLACE)});
  EXPECT_TRUE(delta.removed.empty());
}

/**
 * Given a player who acknowledged the first state and a world that did not
 * change since
 * When the next state is due
 * Then a delta is still sent, with no entity listed, so the player keeps
 * acknowledging
 */
TEST_F(DeltaStateSenderFixture, SendsAnEmptyDeltaWhenNothingChanged) {
  addEnemy(MOVING_ENEMY_ID, FIRST_PLACE);
  ConnectedPlayer& player = connect(FIRST_PLAYER_PORT);
  runTicks(TICKS_TO_THE_FIRST_STATE);
  acknowledgeSentTo(player, EVERY_EARLIER_DATAGRAM);

  runTicks(TICKS_PER_STATE);

  const std::vector<DeltaWorldState> parts = partsSentTo(FIRST_PLAYER_PORT);
  ASSERT_EQ(parts.size(), 2U);
  EXPECT_EQ(parts.back().baseDistance, TICKS_PER_STATE);
  EXPECT_TRUE(parts.back().changed.empty());
  EXPECT_TRUE(parts.back().removed.empty());
}

/**
 * Given a player who acknowledged the first state, and an enemy destroyed since
 * When the next state is due
 * Then the id of that enemy is removed and nothing is changed
 */
TEST_F(DeltaStateSenderFixture, ListsADestroyedEntityAsRemoved) {
  const engine::Entity vanishing = addEnemy(MOVING_ENEMY_ID, FIRST_PLACE);
  addEnemy(STILL_ENEMY_ID, SECOND_PLACE);
  ConnectedPlayer& player = connect(FIRST_PLAYER_PORT);
  runTicks(TICKS_TO_THE_FIRST_STATE);
  acknowledgeSentTo(player, EVERY_EARLIER_DATAGRAM);
  destroy(vanishing);

  runTicks(TICKS_PER_STATE);

  const DeltaWorldState delta = partsSentTo(FIRST_PLAYER_PORT).back();
  EXPECT_EQ(delta.removed, std::vector<std::uint32_t>{MOVING_ENEMY_ID});
  EXPECT_TRUE(delta.changed.empty());
}

/**
 * Given a player who acknowledged nothing
 * When two states are due
 * Then both are written in full, with no reference
 */
TEST_F(DeltaStateSenderFixture,
       SendsEveryStateInFullWhileNothingIsAcknowledged) {
  addEnemy(MOVING_ENEMY_ID, FIRST_PLACE);
  addEnemy(STILL_ENEMY_ID, SECOND_PLACE);
  connect(FIRST_PLAYER_PORT);

  runTicks(TICKS_TO_THE_FIRST_STATE + TICKS_PER_STATE);

  const std::vector<DeltaWorldState> parts = partsSentTo(FIRST_PLAYER_PORT);
  ASSERT_EQ(parts.size(), 2U);
  for (const DeltaWorldState& part : parts) {
    EXPECT_EQ(part.baseDistance, NO_REFERENCE_DISTANCE);
    EXPECT_EQ(part.changed.size(), 2U);
  }
}

/**
 * Given a player who acknowledged the first state only
 * When states go on being sent until one is 60 ticks after the first, then one
 * more
 * Then the state at 60 ticks is still a delta against the first, and the
 * next, 63 ticks after it, is written in full
 */
TEST_F(DeltaStateSenderFixture, FallsBackToAFullStateOnceTheReferenceIsTooOld) {
  addEnemy(MOVING_ENEMY_ID, FIRST_PLACE);
  ConnectedPlayer& player = connect(FIRST_PLAYER_PORT);
  runTicks(TICKS_TO_THE_FIRST_STATE);
  acknowledgeSentTo(player, EVERY_EARLIER_DATAGRAM);

  runTicks(TICKS_TO_THE_FARTHEST_REFERENCE);
  const DeltaWorldState atTheReach = partsSentTo(FIRST_PLAYER_PORT).back();
  runTicks(TICKS_PER_STATE);
  const DeltaWorldState beyondTheReach = partsSentTo(FIRST_PLAYER_PORT).back();

  EXPECT_EQ(atTheReach.baseDistance, network::MAXIMUM_REFERENCE_DISTANCE);
  EXPECT_EQ(beyondTheReach.baseDistance, NO_REFERENCE_DISTANCE);
  EXPECT_EQ(beyondTheReach.changed.size(), 1U);
}

/**
 * Given a state of three datagrams of which the player acknowledged only the
 * last
 * When the next state is due
 * Then it is written in full, since the first two parts may be lost
 */
TEST_F(DeltaStateSenderFixture, DoesNotUseAStateOfWhichAPartIsNotAcknowledged) {
  for (std::size_t index = 0; index < ENEMIES_FOR_THREE_PARTS; ++index) {
    addEnemy(static_cast<std::uint32_t>(index + 1), FIRST_PLACE);
  }
  ConnectedPlayer& player = connect(FIRST_PLAYER_PORT);
  runTicks(TICKS_TO_THE_FIRST_STATE);
  acknowledgeSentTo(player, ONLY_THE_NEWEST_DATAGRAM);

  runTicks(TICKS_PER_STATE);

  const std::vector<DeltaWorldState> parts = partsSentTo(FIRST_PLAYER_PORT);
  std::size_t listedInFull = 0;
  for (std::size_t index = parts.size() / 2; index < parts.size(); ++index) {
    EXPECT_EQ(parts.at(index).baseDistance, NO_REFERENCE_DISTANCE);
    listedInFull += parts.at(index).changed.size();
  }
  EXPECT_EQ(listedInFull, ENEMIES_FOR_THREE_PARTS);
}

/**
 * Given two players of whom only the first acknowledged the first state
 * When the next state is due
 * Then the first gets a delta and the second a state in full
 */
TEST_F(DeltaStateSenderFixture, WritesEachPlayerAgainstItsOwnAcknowledgement) {
  addEnemy(MOVING_ENEMY_ID, FIRST_PLACE);
  ConnectedPlayer& first = connect(FIRST_PLAYER_PORT);
  connect(SECOND_PLAYER_PORT);
  runTicks(TICKS_TO_THE_FIRST_STATE);
  acknowledgeSentTo(first, EVERY_EARLIER_DATAGRAM);

  runTicks(TICKS_PER_STATE);

  EXPECT_EQ(partsSentTo(FIRST_PLAYER_PORT).back().baseDistance,
            TICKS_PER_STATE);
  EXPECT_EQ(partsSentTo(SECOND_PLAYER_PORT).back().baseDistance,
            NO_REFERENCE_DISTANCE);
}

}  // namespace rtype::server::testing
