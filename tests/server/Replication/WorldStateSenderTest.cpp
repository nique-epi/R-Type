#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "EntityType.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateSenderFixture.hpp"
#include "WorldStateSenderTestConstants.hpp"

using rtype::game::EntityType;
using rtype::network::EntityState;
using rtype::network::WireEntityType;
using rtype::server::testing::client;
using rtype::server::testing::ENEMY_MISSILE_NETWORK_ID;
using rtype::server::testing::ENEMY_MISSILE_POSITION;
using rtype::server::testing::ENEMY_NETWORK_ID;
using rtype::server::testing::ENEMY_POSITION;
using rtype::server::testing::expectedEntity;
using rtype::server::testing::FIRST_CLIENT_PORT;
using rtype::server::testing::INFINITE_POSITION;
using rtype::server::testing::MANY_ENTITIES;
using rtype::server::testing::NOT_A_NUMBER_POSITION;
using rtype::server::testing::OTHER_SESSION_TOKEN;
using rtype::server::testing::PLAYER_MISSILE_NETWORK_ID;
using rtype::server::testing::PLAYER_MISSILE_POSITION;
using rtype::server::testing::PLAYER_NETWORK_ID;
using rtype::server::testing::PLAYER_POSITION;
using rtype::server::testing::SECOND_CLIENT_PORT;
using rtype::server::testing::SESSION_TOKEN;
using rtype::server::testing::SHIP_NETWORK_ID;
using rtype::server::testing::SHIP_POSITION;
using rtype::server::testing::UNTYPED_NETWORK_ID;
using rtype::server::testing::WorldStateSenderFixture;

/**
 * Given a world with one networked ship and one connected player
 * When one second of ticks runs
 * Then WORLD_STATES_PER_SECOND states are sent, with the ticks
 * TICKS_PER_WORLD_STATE, 2 times that, and so on
 */
TEST_F(WorldStateSenderFixture, SendsTheStatesPerSecondOfTheConstant) {
  addNetworkedEntity(EntityType::Player, SHIP_POSITION, SHIP_NETWORK_ID);
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  runTicks(rtype::engine::SIMULATION_TICKS_PER_SECOND);

  ASSERT_EQ(sentDatagrams().size(), rtype::server::WORLD_STATES_PER_SECOND);
  for (std::size_t index = 0; index < sentDatagrams().size(); ++index) {
    EXPECT_EQ(decodedState(index).tick,
              (index + 1) * rtype::server::TICKS_PER_WORLD_STATE);
  }
}

/**
 * Given a world with one networked ship and one connected player
 * When fewer ticks than TICKS_PER_WORLD_STATE run
 * Then nothing is sent
 */
TEST_F(WorldStateSenderFixture, SendsNothingBeforeTheFirstStateIsDue) {
  addNetworkedEntity(EntityType::Player, SHIP_POSITION, SHIP_NETWORK_ID);
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  runTicks(rtype::server::TICKS_PER_WORLD_STATE - 1);

  EXPECT_TRUE(sentDatagrams().empty());
}

/**
 * Given a world with one networked ship and no connected player
 * When a second of ticks runs
 * Then nothing is sent
 */
TEST_F(WorldStateSenderFixture, SendsNothingWithoutPlayers) {
  addNetworkedEntity(EntityType::Player, SHIP_POSITION, SHIP_NETWORK_ID);

  runTicks(rtype::engine::SIMULATION_TICKS_PER_SECOND);

  EXPECT_TRUE(sentDatagrams().empty());
}

/**
 * Given two connected players
 * When the first state is due
 * Then each gets one datagram, holding the same state
 */
TEST_F(WorldStateSenderFixture, SendsTheStateToEveryPlayer) {
  addNetworkedEntity(EntityType::Player, SHIP_POSITION, SHIP_NETWORK_ID);
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);
  connect(SECOND_CLIENT_PORT, OTHER_SESSION_TOKEN);

  runTicks(rtype::server::TICKS_PER_WORLD_STATE);

  ASSERT_EQ(sentDatagrams().size(), 2U);
  EXPECT_EQ(sentDatagrams()[0].destination, client(FIRST_CLIENT_PORT));
  EXPECT_EQ(sentDatagrams()[1].destination, client(SECOND_CLIENT_PORT));
  EXPECT_EQ(decodedState(0), decodedState(1));
}

/**
 * Given two connected players with their own session tokens
 * When the first state is due
 * Then the datagram to each starts with its token and the first sequence of
 * its session
 */
TEST_F(WorldStateSenderFixture, StartsEachDatagramWithTheHeaderOfItsPlayer) {
  addNetworkedEntity(EntityType::Player, SHIP_POSITION, SHIP_NETWORK_ID);
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);
  connect(SECOND_CLIENT_PORT, OTHER_SESSION_TOKEN);

  runTicks(rtype::server::TICKS_PER_WORLD_STATE);

  ASSERT_EQ(sentDatagrams().size(), 2U);
  EXPECT_EQ(headerOf(0).sessionToken, SESSION_TOKEN);
  EXPECT_EQ(headerOf(0).sequence, 0);
  EXPECT_EQ(headerOf(1).sessionToken, OTHER_SESSION_TOKEN);
  EXPECT_EQ(headerOf(1).sequence, 0);
}

/**
 * Given entities of the four types
 * When the first state is due
 * Then it lists each under its network id, its wire type and its position
 */
TEST_F(WorldStateSenderFixture, ListsEachEntityUnderItsWireType) {
  addNetworkedEntity(EntityType::Player, PLAYER_POSITION, PLAYER_NETWORK_ID);
  addNetworkedEntity(EntityType::Enemy, ENEMY_POSITION, ENEMY_NETWORK_ID);
  addNetworkedEntity(EntityType::PlayerMissile, PLAYER_MISSILE_POSITION,
                     PLAYER_MISSILE_NETWORK_ID);
  addNetworkedEntity(EntityType::EnemyMissile, ENEMY_MISSILE_POSITION,
                     ENEMY_MISSILE_NETWORK_ID);
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  runTicks(rtype::server::TICKS_PER_WORLD_STATE);

  ASSERT_EQ(sentDatagrams().size(), 1U);
  std::vector<EntityState> listed = decodedState(0).entities;
  std::ranges::sort(listed, {}, &EntityState::entityId);
  const std::vector<EntityState> expected{
      expectedEntity(PLAYER_NETWORK_ID, WireEntityType::PlayerShip,
                     PLAYER_POSITION),
      expectedEntity(ENEMY_NETWORK_ID, WireEntityType::Bydo, ENEMY_POSITION),
      expectedEntity(PLAYER_MISSILE_NETWORK_ID, WireEntityType::PlayerMissile,
                     PLAYER_MISSILE_POSITION),
      expectedEntity(ENEMY_MISSILE_NETWORK_ID, WireEntityType::EnemyMissile,
                     ENEMY_MISSILE_POSITION)};
  EXPECT_EQ(listed, expected);
}

/**
 * Given a ship, an entity without a network id and an entity without a type
 * When the first state is due
 * Then only the ship is listed, because the others cannot be named or typed
 */
TEST_F(WorldStateSenderFixture, ListsOnlyEntitiesThatCanBeNamedAndTyped) {
  addNetworkedEntity(EntityType::Player, SHIP_POSITION, SHIP_NETWORK_ID);
  addEntityWithoutNetworkId(EntityType::Player, PLAYER_POSITION);
  addEntityWithoutType(ENEMY_POSITION, UNTYPED_NETWORK_ID);
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  runTicks(rtype::server::TICKS_PER_WORLD_STATE);

  ASSERT_EQ(sentDatagrams().size(), 1U);
  EXPECT_EQ(decodedState(0).entities,
            (std::vector<EntityState>{expectedEntity(
                SHIP_NETWORK_ID, WireEntityType::PlayerShip, SHIP_POSITION)}));
}

/**
 * Given one connected player and more networked entities than one datagram
 * holds
 * When the first state is due
 * Then several datagrams are sent, all with the same tick, none above the
 * datagram size limit, and together they list every entity once
 */
TEST_F(WorldStateSenderFixture, SplitsABigStateIntoDatagramsOfTheSameTick) {
  for (std::size_t index = 0; index < MANY_ENTITIES; ++index) {
    addNetworkedEntity(EntityType::Enemy, SHIP_POSITION,
                       static_cast<std::uint32_t>(index + 1));
  }
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  runTicks(rtype::server::TICKS_PER_WORLD_STATE);

  ASSERT_GT(sentDatagrams().size(), 1U);
  std::size_t listed = 0;
  for (std::size_t index = 0; index < sentDatagrams().size(); ++index) {
    EXPECT_LE(sentDatagrams()[index].payload.size(),
              rtype::network::MAX_DATAGRAM_SIZE);
    EXPECT_EQ(decodedState(index).tick, rtype::server::TICKS_PER_WORLD_STATE);
    listed += decodedState(index).entities.size();
  }
  EXPECT_EQ(listed, MANY_ENTITIES);
}

/**
 * Given one connected player and more networked entities than one datagram
 * holds
 * When the first state is due
 * Then the parts carry the sequences 0, 1, 2… of the session of the player,
 * one each, so the client reads every part
 */
TEST_F(WorldStateSenderFixture, GivesEachPartOfAStateTheNextSequence) {
  for (std::size_t index = 0; index < MANY_ENTITIES; ++index) {
    addNetworkedEntity(EntityType::Enemy, SHIP_POSITION,
                       static_cast<std::uint32_t>(index + 1));
  }
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  runTicks(rtype::server::TICKS_PER_WORLD_STATE);

  ASSERT_GT(sentDatagrams().size(), 1U);
  for (std::size_t index = 0; index < sentDatagrams().size(); ++index) {
    EXPECT_EQ(headerOf(index).sequence, static_cast<std::uint16_t>(index));
  }
}

/**
 * Given a ship, and a Bydo whose position is not a number after a game bug
 * When the first state is due
 * Then the state is sent and lists the ship only: the server keeps running
 */
TEST_F(WorldStateSenderFixture, LeavesOutAnEntityWhosePositionIsNotANumber) {
  addNetworkedEntity(EntityType::Player, SHIP_POSITION, SHIP_NETWORK_ID);
  addNetworkedEntity(EntityType::Enemy, NOT_A_NUMBER_POSITION,
                     ENEMY_NETWORK_ID);
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  runTicks(rtype::server::TICKS_PER_WORLD_STATE);

  ASSERT_EQ(sentDatagrams().size(), 1U);
  EXPECT_EQ(decodedState(0).entities,
            std::vector{expectedEntity(
                SHIP_NETWORK_ID, WireEntityType::PlayerShip, SHIP_POSITION)});
}

/**
 * Given a ship, and a Bydo whose position is infinite after a game bug
 * When the first state is due
 * Then the state is sent and lists the ship only
 */
TEST_F(WorldStateSenderFixture, LeavesOutAnEntityWhosePositionIsInfinite) {
  addNetworkedEntity(EntityType::Player, SHIP_POSITION, SHIP_NETWORK_ID);
  addNetworkedEntity(EntityType::Enemy, INFINITE_POSITION, ENEMY_NETWORK_ID);
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  runTicks(rtype::server::TICKS_PER_WORLD_STATE);

  ASSERT_EQ(sentDatagrams().size(), 1U);
  EXPECT_EQ(decodedState(0).entities,
            std::vector{expectedEntity(
                SHIP_NETWORK_ID, WireEntityType::PlayerShip, SHIP_POSITION)});
}
