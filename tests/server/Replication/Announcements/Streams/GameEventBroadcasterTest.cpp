#include <gtest/gtest.h>
#include <vector>
#include "EnemyAction.hpp"
#include "EnemyActionStarted.hpp"
#include "EntityDamage.hpp"
#include "EntityDestroyed.hpp"
#include "EntityDestruction.hpp"
#include "EntitySpawn.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "GameEvent.hpp"
#include "GameEventBroadcasterFixture.hpp"
#include "GameEventConstants.hpp"
#include "GameEventTestConstants.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"
#include "PlayerLeft.hpp"
#include "WorldState.hpp"

namespace rtype::server::testing {

/**
 * Given a broadcaster
 * When a monster spawn is published
 * Then one EntitySpawned with its id, type and position is sent once to every
 * player, and nothing is guaranteed
 */
TEST_F(GameEventBroadcasterFixture, SendsASpawnOnce) {
  publishAndDispatch(game::EntitySpawn{.networkId = MONSTER_NETWORK_ID,
                                       .type = game::EntityType::Enemy,
                                       .x = MONSTER_X,
                                       .y = MONSTER_Y});

  const std::vector<network::EntitySpawned> expected{
      {.entityId = MONSTER_NETWORK_ID,
       .entityType = network::WireEntityType::Bydo,
       .x = MONSTER_X,
       .y = MONSTER_Y}};
  EXPECT_EQ(sentOnce<network::EntitySpawned>(), expected);
  EXPECT_TRUE(guaranteed<network::EntitySpawned>().empty());
}

/**
 * Given a broadcaster
 * When a kill is published
 * Then one EntityDestroyed, Killed, is guaranteed to every player, and none is
 * sent once
 */
TEST_F(GameEventBroadcasterFixture, GuaranteesADestruction) {
  publishAndDispatch(
      game::EntityDestruction{.networkId = MONSTER_NETWORK_ID,
                              .cause = game::DestructionCause::Killed});

  const std::vector<network::EntityDestroyed> expected{
      {.entityId = MONSTER_NETWORK_ID,
       .reason = network::DestructionReason::Killed}};
  EXPECT_EQ(guaranteed<network::EntityDestroyed>(), expected);
  EXPECT_TRUE(sentOnce<network::EntityDestroyed>().empty());
}

/**
 * Given a broadcaster
 * When a player hit is published
 * Then one GameEvent of kind PlayerHit, numbered FIRST_EVENT_NUMBER, is
 * guaranteed
 */
TEST_F(GameEventBroadcasterFixture, GuaranteesAGameEventWithTheFirstNumber) {
  publishAndDispatch(game::PlayerHit{.playerNumber = SECOND_PLAYER_NUMBER});

  const std::vector<network::GameEvent> expected{
      {.eventNumber = FIRST_EVENT_NUMBER,
       .kind = network::GameEventKind::PlayerHit,
       .playerNumber = SECOND_PLAYER_NUMBER,
       .entityId = 0}};
  EXPECT_EQ(guaranteed<network::GameEvent>(), expected);
}

/**
 * Given a broadcaster
 * When a player that timed out is published
 * Then one PlayerLeft naming that player, TimedOut, is guaranteed
 */
TEST_F(GameEventBroadcasterFixture, GuaranteesADeparture) {
  publishAndDispatch(
      game::PlayerDeparture{.playerNumber = SECOND_PLAYER_NUMBER,
                            .cause = game::DepartureCause::TimedOut});

  const std::vector<network::PlayerLeft> expected{
      {.playerNumber = SECOND_PLAYER_NUMBER,
       .reason = network::DepartureReason::TimedOut}};
  EXPECT_EQ(guaranteed<network::PlayerLeft>(), expected);
}

/**
 * Given a broadcaster
 * When a spawn whose position is not a number is published, then a valid spawn
 * Then dispatching does not throw, the first spawn sends nothing, and the
 * second is sent once
 */
TEST_F(GameEventBroadcasterFixture, SkipsASpawnWhoseCoordinateIsNotANumber) {
  EXPECT_NO_THROW(
      publishAndDispatch(game::EntitySpawn{.networkId = MONSTER_NETWORK_ID,
                                           .type = game::EntityType::Enemy,
                                           .x = NOT_A_NUMBER,
                                           .y = MONSTER_Y}));
  EXPECT_EQ(messageCount(), 0U);

  publishAndDispatch(game::EntitySpawn{.networkId = MONSTER_NETWORK_ID,
                                       .type = game::EntityType::Enemy,
                                       .x = MONSTER_X,
                                       .y = MONSTER_Y});

  EXPECT_EQ(sentOnce<network::EntitySpawned>().size(), 1U);
}

/**
 * Given a broadcaster
 * When an entity damage naming the network id 0 is published, then a player
 * hit
 * Then dispatching does not throw, the damage is never sent, and the player
 * hit carries the first event number
 */
TEST_F(GameEventBroadcasterFixture,
       SkipsAnEventWithoutAnEntityIdAndKeepsItsNumber) {
  EXPECT_NO_THROW(publishAndDispatch(
      game::EntityDamage{.networkId = UNASSIGNED_NETWORK_ID}));
  EXPECT_EQ(messageCount(), 0U);

  publishAndDispatch(game::PlayerHit{.playerNumber = SECOND_PLAYER_NUMBER});

  const std::vector<network::GameEvent> received =
      guaranteed<network::GameEvent>();
  ASSERT_EQ(received.size(), 1U);
  EXPECT_EQ(received.front().eventNumber, FIRST_EVENT_NUMBER);
  EXPECT_EQ(received.front().kind, network::GameEventKind::PlayerHit);
}

/**
 * Given a broadcaster
 * When a player hit for a player outside the game is published, then a player
 * elimination for a player of the game
 * Then dispatching does not throw, the hit sends nothing, and the elimination
 * is guaranteed
 */
TEST_F(GameEventBroadcasterFixture, SkipsAPlayerEventOfAPlayerOutsideTheGame) {
  EXPECT_NO_THROW(publishAndDispatch(
      game::PlayerHit{.playerNumber = PLAYER_NUMBER_OUTSIDE_THE_GAME}));
  EXPECT_EQ(messageCount(), 0U);

  publishAndDispatch(
      game::PlayerElimination{.playerNumber = SECOND_PLAYER_NUMBER});

  EXPECT_EQ(guaranteed<network::GameEvent>().size(), 1U);
}

/**
 * Given a broadcaster
 * When the departure of a player outside the game is published
 * Then dispatching does not throw and nothing is sent
 */
TEST_F(GameEventBroadcasterFixture, SkipsTheDepartureOfAPlayerOutsideTheGame) {
  EXPECT_NO_THROW(publishAndDispatch(
      game::PlayerDeparture{.playerNumber = PLAYER_NUMBER_OUTSIDE_THE_GAME,
                            .cause = game::DepartureCause::TimedOut}));

  EXPECT_EQ(messageCount(), 0U);
}

/**
 * Given a broadcaster that was destroyed
 * When a destruction is published and the bus delivers it
 * Then nothing is sent
 */
TEST_F(GameEventBroadcasterFixture, StopsListeningOnceDestroyed) {
  destroyBroadcaster();

  publishAndDispatch(
      game::EntityDestruction{.networkId = MONSTER_NETWORK_ID,
                              .cause = game::DestructionCause::Killed});

  EXPECT_EQ(messageCount(), 0U);
}

/**
 * Given a broadcaster
 * When an enemy entering the phase Signal of attack 1 is published
 * Then one EnemyAction naming it is guaranteed to every player, and none is
 * sent once
 */
TEST_F(GameEventBroadcasterFixture, GuaranteesAnEnemyAction) {
  publishAndDispatch(
      game::EnemyActionStarted{.networkId = MONSTER_NETWORK_ID,
                               .attack = 1,
                               .phase = game::EnemyPhase::Signal});

  const std::vector<network::EnemyAction> expected{
      {.entityId = MONSTER_NETWORK_ID,
       .attack = 1,
       .phase = network::EnemyActionPhase::Signal}};
  EXPECT_EQ(guaranteed<network::EnemyAction>(), expected);
  EXPECT_TRUE(sentOnce<network::EnemyAction>().empty());
}

}  // namespace rtype::server::testing
