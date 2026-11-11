#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "AppliedInput.hpp"
#include "ConnectedPlayer.hpp"
#include "EntityTypeNumberTestConstants.hpp"
#include "Invulnerable.hpp"
#include "Lives.hpp"
#include "NetworkConstants.hpp"
#include "ProtocolConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerException.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ServerTestConstants.hpp"
#include "StateRateMeasurementFixtures.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateSenderFixture.hpp"
#include "WorldStateSenderTestConstants.hpp"

using rtype::game::Invulnerable;
using rtype::game::Lives;
using rtype::network::EntityState;
using rtype::network::NO_SHOWN_TICK;
using rtype::network::PlayerState;
using rtype::server::testing::client;
using rtype::server::testing::DRIFTER_TYPE_NUMBER;
using rtype::server::testing::ENEMY_MISSILE_NETWORK_ID;
using rtype::server::testing::ENEMY_MISSILE_POSITION;
using rtype::server::testing::ENEMY_MISSILE_TYPE_NUMBER;
using rtype::server::testing::ENEMY_NETWORK_ID;
using rtype::server::testing::ENEMY_POSITION;
using rtype::server::testing::FIRST_CLIENT_PORT;
using rtype::server::testing::FIRST_LIVES;
using rtype::server::testing::FIRST_SCORE;
using rtype::server::testing::FIRST_SHIP_ID;
using rtype::server::testing::INFINITE_POSITION;
using rtype::server::testing::listedEntity;
using rtype::server::testing::MANY_ENTITIES;
using rtype::server::testing::NEWEST_INPUT_NUMBER;
using rtype::server::testing::NO_LOOK;
using rtype::server::testing::NOT_A_NUMBER_POSITION;
using rtype::server::testing::ONE_TICK_OF_PROTECTION;
using rtype::server::testing::OTHER_SESSION_TOKEN;
using rtype::server::testing::PLAYER_MISSILE_NETWORK_ID;
using rtype::server::testing::PLAYER_MISSILE_POSITION;
using rtype::server::testing::PLAYER_MISSILE_TYPE_NUMBER;
using rtype::server::testing::PLAYER_NETWORK_ID;
using rtype::server::testing::PLAYER_POSITION;
using rtype::server::testing::RECEIVED_INPUT_NUMBER;
using rtype::server::testing::REFUSED_STATE_RATES;
using rtype::server::testing::SECOND_CLIENT_PORT;
using rtype::server::testing::SECOND_LIVES;
using rtype::server::testing::SECOND_SCORE;
using rtype::server::testing::SECOND_SHIP_ID;
using rtype::server::testing::SESSION_TOKEN;
using rtype::server::testing::SHIP_NETWORK_ID;
using rtype::server::testing::SHIP_POSITION;
using rtype::server::testing::SHIP_TYPE_NUMBER;
using rtype::server::testing::SHOWN_LOOK;
using rtype::server::testing::STATE_RATES_UNDER_TEST;
using rtype::server::testing::ticksSentTo;
using rtype::server::testing::UNTYPED_NETWORK_ID;
using rtype::server::testing::WorldStateSenderFixture;

/**
 * Given a world with one networked ship and one connected player
 * When one second of ticks runs
 * Then DEFAULT_WORLD_STATES_PER_SECOND states are sent, with the ticks
 * DEFAULT_TICKS_PER_WORLD_STATE, 2 times that, and so on
 */
TEST_F(WorldStateSenderFixture, SendsTheStatesPerSecondOfTheConstant) {
  addNetworkedEntity(
      listedEntity(SHIP_NETWORK_ID, SHIP_TYPE_NUMBER, SHIP_POSITION));
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  runTicks(rtype::engine::SIMULATION_TICKS_PER_SECOND);

  ASSERT_EQ(sentDatagrams().size(),
            rtype::server::DEFAULT_WORLD_STATES_PER_SECOND);
  for (std::size_t index = 0; index < sentDatagrams().size(); ++index) {
    EXPECT_EQ(decodedState(index).tick,
              (index + 1) * DEFAULT_TICKS_PER_WORLD_STATE);
  }
}

/**
 * Given a world with one networked ship and one connected player
 * When fewer ticks than DEFAULT_TICKS_PER_WORLD_STATE run
 * Then nothing is sent
 */
TEST_F(WorldStateSenderFixture, SendsNothingBeforeTheFirstStateIsDue) {
  addNetworkedEntity(
      listedEntity(SHIP_NETWORK_ID, SHIP_TYPE_NUMBER, SHIP_POSITION));
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  runTicks(DEFAULT_TICKS_PER_WORLD_STATE - 1);

  EXPECT_TRUE(sentDatagrams().empty());
}

/**
 * Given a world with one networked ship and no connected player
 * When a second of ticks runs
 * Then nothing is sent
 */
TEST_F(WorldStateSenderFixture, SendsNothingWithoutPlayers) {
  addNetworkedEntity(
      listedEntity(SHIP_NETWORK_ID, SHIP_TYPE_NUMBER, SHIP_POSITION));

  runTicks(rtype::engine::SIMULATION_TICKS_PER_SECOND);

  EXPECT_TRUE(sentDatagrams().empty());
}

/**
 * Given two connected players
 * When the first state is due
 * Then each gets one datagram, holding the same state
 */
TEST_F(WorldStateSenderFixture, SendsTheStateToEveryPlayer) {
  addNetworkedEntity(
      listedEntity(SHIP_NETWORK_ID, SHIP_TYPE_NUMBER, SHIP_POSITION));
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);
  connect(SECOND_CLIENT_PORT, OTHER_SESSION_TOKEN);

  runTicks(DEFAULT_TICKS_PER_WORLD_STATE);

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
  addNetworkedEntity(
      listedEntity(SHIP_NETWORK_ID, SHIP_TYPE_NUMBER, SHIP_POSITION));
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);
  connect(SECOND_CLIENT_PORT, OTHER_SESSION_TOKEN);

  runTicks(DEFAULT_TICKS_PER_WORLD_STATE);

  ASSERT_EQ(sentDatagrams().size(), 2U);
  EXPECT_EQ(headerOf(0).sessionToken, SESSION_TOKEN);
  EXPECT_EQ(headerOf(0).sequence, 0);
  EXPECT_EQ(headerOf(1).sessionToken, OTHER_SESSION_TOKEN);
  EXPECT_EQ(headerOf(1).sequence, 0);
}

/**
 * Given entities of the four types
 * When the first state is due
 * Then it lists each under its network id, the number of its type and its
 * position
 */
TEST_F(WorldStateSenderFixture, ListsEachEntityUnderItsTypeNumber) {
  addNetworkedEntity(
      listedEntity(PLAYER_NETWORK_ID, SHIP_TYPE_NUMBER, PLAYER_POSITION));
  addNetworkedEntity(
      listedEntity(ENEMY_NETWORK_ID, DRIFTER_TYPE_NUMBER, ENEMY_POSITION));
  addNetworkedEntity(listedEntity(PLAYER_MISSILE_NETWORK_ID,
                                  PLAYER_MISSILE_TYPE_NUMBER,
                                  PLAYER_MISSILE_POSITION));
  addNetworkedEntity(listedEntity(ENEMY_MISSILE_NETWORK_ID,
                                  ENEMY_MISSILE_TYPE_NUMBER,
                                  ENEMY_MISSILE_POSITION));
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  runTicks(DEFAULT_TICKS_PER_WORLD_STATE);

  ASSERT_EQ(sentDatagrams().size(), 1U);
  std::vector<EntityState> listed = decodedState(0).entities;
  std::ranges::sort(listed, {}, &EntityState::entityId);
  const std::vector<EntityState> expected{
      listedEntity(PLAYER_NETWORK_ID, SHIP_TYPE_NUMBER, PLAYER_POSITION),
      listedEntity(ENEMY_NETWORK_ID, DRIFTER_TYPE_NUMBER, ENEMY_POSITION),
      listedEntity(PLAYER_MISSILE_NETWORK_ID, PLAYER_MISSILE_TYPE_NUMBER,
                   PLAYER_MISSILE_POSITION),
      listedEntity(ENEMY_MISSILE_NETWORK_ID, ENEMY_MISSILE_TYPE_NUMBER,
                   ENEMY_MISSILE_POSITION)};
  EXPECT_EQ(listed, expected);
}

/**
 * Given a Bydo that shows a look, and a ship that shows none
 * When the first state is due
 * Then the Bydo is listed with its look, the ship with look 0
 */
TEST_F(WorldStateSenderFixture, ListsTheLookEachEntityShows) {
  EntityState bydo =
      listedEntity(ENEMY_NETWORK_ID, DRIFTER_TYPE_NUMBER, ENEMY_POSITION);
  bydo.look = SHOWN_LOOK;
  addNetworkedEntity(bydo);
  addNetworkedEntity(
      listedEntity(SHIP_NETWORK_ID, SHIP_TYPE_NUMBER, SHIP_POSITION));
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  runTicks(DEFAULT_TICKS_PER_WORLD_STATE);

  ASSERT_EQ(sentDatagrams().size(), 1U);
  std::vector<EntityState> listed = decodedState(0).entities;
  std::ranges::sort(listed, {}, &EntityState::entityId);
  ASSERT_EQ(listed.size(), 2U);
  EXPECT_EQ(listed.front().look, SHOWN_LOOK);
  EXPECT_EQ(listed.back().look, NO_LOOK);
}

/**
 * Given a ship, an entity without a network id and an entity without a type
 * When the first state is due
 * Then only the ship is listed, because the others cannot be named or typed
 */
TEST_F(WorldStateSenderFixture, ListsOnlyEntitiesThatCanBeNamedAndTyped) {
  addNetworkedEntity(
      listedEntity(SHIP_NETWORK_ID, SHIP_TYPE_NUMBER, SHIP_POSITION));
  addEntityWithoutNetworkId(SHIP_TYPE_NUMBER, PLAYER_POSITION);
  addEntityWithoutType(ENEMY_POSITION, UNTYPED_NETWORK_ID);
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  runTicks(DEFAULT_TICKS_PER_WORLD_STATE);

  ASSERT_EQ(sentDatagrams().size(), 1U);
  EXPECT_EQ(decodedState(0).entities,
            (std::vector<EntityState>{listedEntity(
                SHIP_NETWORK_ID, SHIP_TYPE_NUMBER, SHIP_POSITION)}));
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
    addNetworkedEntity(listedEntity(static_cast<std::uint32_t>(index + 1),
                                    DRIFTER_TYPE_NUMBER, SHIP_POSITION));
  }
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  runTicks(DEFAULT_TICKS_PER_WORLD_STATE);

  ASSERT_GT(sentDatagrams().size(), 1U);
  std::size_t listed = 0;
  for (std::size_t index = 0; index < sentDatagrams().size(); ++index) {
    EXPECT_LE(sentDatagrams()[index].payload.size(),
              rtype::network::MAX_DATAGRAM_SIZE);
    EXPECT_EQ(decodedState(index).tick, DEFAULT_TICKS_PER_WORLD_STATE);
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
    addNetworkedEntity(listedEntity(static_cast<std::uint32_t>(index + 1),
                                    DRIFTER_TYPE_NUMBER, SHIP_POSITION));
  }
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  runTicks(DEFAULT_TICKS_PER_WORLD_STATE);

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
  addNetworkedEntity(
      listedEntity(SHIP_NETWORK_ID, SHIP_TYPE_NUMBER, SHIP_POSITION));
  addNetworkedEntity(listedEntity(ENEMY_NETWORK_ID, DRIFTER_TYPE_NUMBER,
                                  NOT_A_NUMBER_POSITION));
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  runTicks(DEFAULT_TICKS_PER_WORLD_STATE);

  ASSERT_EQ(sentDatagrams().size(), 1U);
  EXPECT_EQ(decodedState(0).entities,
            std::vector{listedEntity(SHIP_NETWORK_ID, SHIP_TYPE_NUMBER,
                                     SHIP_POSITION)});
}

/**
 * Given a ship, and a Bydo whose position is infinite after a game bug
 * When the first state is due
 * Then the state is sent and lists the ship only
 */
TEST_F(WorldStateSenderFixture, LeavesOutAnEntityWhosePositionIsInfinite) {
  addNetworkedEntity(
      listedEntity(SHIP_NETWORK_ID, SHIP_TYPE_NUMBER, SHIP_POSITION));
  addNetworkedEntity(
      listedEntity(ENEMY_NETWORK_ID, DRIFTER_TYPE_NUMBER, INFINITE_POSITION));
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  runTicks(DEFAULT_TICKS_PER_WORLD_STATE);

  ASSERT_EQ(sentDatagrams().size(), 1U);
  EXPECT_EQ(decodedState(0).entities,
            std::vector{listedEntity(SHIP_NETWORK_ID, SHIP_TYPE_NUMBER,
                                     SHIP_POSITION)});
}

/**
 * Given two connected players with different ships, lives and scores
 * When the first state is due
 * Then the state lists both, each under its number with its own ship, lives
 * and score, and every player receives the same list
 */
TEST_F(WorldStateSenderFixture, ListsEveryPlayerWithItsOwnShipLivesAndScore) {
  rtype::server::ConnectedPlayer& first =
      connect(FIRST_CLIENT_PORT, SESSION_TOKEN);
  rtype::server::ConnectedPlayer& second =
      connect(SECOND_CLIENT_PORT, OTHER_SESSION_TOKEN);
  first.shipEntityId = FIRST_SHIP_ID;
  first.score = FIRST_SCORE;
  second.shipEntityId = SECOND_SHIP_ID;
  second.score = SECOND_SCORE;
  addShip(FIRST_SHIP_ID, Lives{.count = FIRST_LIVES}, Invulnerable{});
  addShip(SECOND_SHIP_ID, Lives{.count = SECOND_LIVES}, Invulnerable{});

  runTicks(DEFAULT_TICKS_PER_WORLD_STATE);

  ASSERT_EQ(sentDatagrams().size(), 2U);
  const std::vector<PlayerState> expected{
      PlayerState{.playerNumber = first.playerNumber,
                  .shipEntityId = FIRST_SHIP_ID,
                  .lives = FIRST_LIVES,
                  .score = FIRST_SCORE,
                  .invulnerable = false},
      PlayerState{.playerNumber = second.playerNumber,
                  .shipEntityId = SECOND_SHIP_ID,
                  .lives = SECOND_LIVES,
                  .score = SECOND_SCORE,
                  .invulnerable = false}};
  EXPECT_EQ(decodedState(0).players, expected);
  EXPECT_EQ(decodedState(1).players, expected);
}

/**
 * Given a player without a ship, who keeps lives for its next one
 * When the first state is due
 * Then the state lists the player with those lives, not invulnerable
 */
TEST_F(WorldStateSenderFixture, ListsAPlayerWithoutAShipWithTheLivesItKeeps) {
  rtype::server::ConnectedPlayer& player =
      connect(FIRST_CLIENT_PORT, SESSION_TOKEN);
  player.livesWithoutShip = FIRST_LIVES;

  runTicks(DEFAULT_TICKS_PER_WORLD_STATE);

  ASSERT_EQ(decodedState(0).players.size(), 1U);
  EXPECT_EQ(decodedState(0).players.front().lives, FIRST_LIVES);
  EXPECT_FALSE(decodedState(0).players.front().invulnerable);
}

/**
 * Given a player whose ship came back after a hit and has one tick of
 * protection left
 * When the first state is due
 * Then the state lists the player as invulnerable
 */
TEST_F(WorldStateSenderFixture, ListsAPlayerWithProtectionLeftAsInvulnerable) {
  rtype::server::ConnectedPlayer& player =
      connect(FIRST_CLIENT_PORT, SESSION_TOKEN);
  player.shipEntityId = FIRST_SHIP_ID;
  addShip(FIRST_SHIP_ID, Lives{.count = FIRST_LIVES},
          Invulnerable{.ticksLeft = ONE_TICK_OF_PROTECTION});

  runTicks(DEFAULT_TICKS_PER_WORLD_STATE);

  ASSERT_EQ(sentDatagrams().size(), 1U);
  ASSERT_EQ(decodedState(0).players.size(), 1U);
  EXPECT_TRUE(decodedState(0).players.front().invulnerable);
}

/**
 * Given a player whose input 9 arrived but whose ship only applied input 7,
 * and one who sent none
 * When the first state is due
 * Then the state lists 7 for the first and 0 for the second
 */
TEST_F(WorldStateSenderFixture, ListsTheAppliedInputNumberOfEachPlayer) {
  rtype::server::ConnectedPlayer& sender =
      connect(FIRST_CLIENT_PORT, SESSION_TOKEN);
  const rtype::server::ConnectedPlayer& silent =
      connect(SECOND_CLIENT_PORT, OTHER_SESSION_TOKEN);
  sender.newestInputNumber = RECEIVED_INPUT_NUMBER;
  sender.appliedInput = rtype::server::AppliedInput{
      .inputNumber = NEWEST_INPUT_NUMBER, .shownTick = NO_SHOWN_TICK};

  runTicks(DEFAULT_TICKS_PER_WORLD_STATE);

  const std::vector<PlayerState> players = decodedState(0).players;
  ASSERT_EQ(players.size(), 2U);
  EXPECT_EQ(players[0].playerNumber, sender.playerNumber);
  EXPECT_EQ(players[0].acknowledgedInput, NEWEST_INPUT_NUMBER);
  EXPECT_EQ(players[1].playerNumber, silent.playerNumber);
  EXPECT_EQ(players[1].acknowledgedInput, 0U);
}

/**
 * Given a player who has not scored
 * When the first state is due
 * Then the state lists the player with a score of 0
 */
TEST_F(WorldStateSenderFixture, ListsAPlayerWhoHasNotScoredWithZero) {
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  runTicks(DEFAULT_TICKS_PER_WORLD_STATE);

  ASSERT_EQ(sentDatagrams().size(), 1U);
  ASSERT_EQ(decodedState(0).players.size(), 1U);
  EXPECT_EQ(decodedState(0).players.front().score, 0U);
}

/**
 * Given a world with one networked ship and one connected player
 * When one second of ticks runs at each rate under test
 * Then exactly that many states are sent
 */
TEST_F(WorldStateSenderFixture, SendsExactlyTheConfiguredStatesPerSecond) {
  addNetworkedEntity(
      listedEntity(SHIP_NETWORK_ID, SHIP_TYPE_NUMBER, SHIP_POSITION));
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  for (const std::size_t rate : STATE_RATES_UNDER_TEST) {
    SCOPED_TRACE(rate);
    restartWithStatesPerSecond(rate);

    runTicks(rtype::engine::SIMULATION_TICKS_PER_SECOND);

    EXPECT_EQ(sentDatagrams().size(), rate);
  }
}

/**
 * Given a world with one networked ship and one connected player
 * When one second of ticks runs at each rate under test
 * Then no two states are further apart than a rate-th of a second rounded up
 * to whole ticks
 */
TEST_F(WorldStateSenderFixture, SpreadsTheStatesOverTheSecond) {
  addNetworkedEntity(
      listedEntity(SHIP_NETWORK_ID, SHIP_TYPE_NUMBER, SHIP_POSITION));
  connect(FIRST_CLIENT_PORT, SESSION_TOKEN);

  for (const std::size_t rate : STATE_RATES_UNDER_TEST) {
    SCOPED_TRACE(rate);
    restartWithStatesPerSecond(rate);
    const std::size_t longestGapInTicks =
        (rtype::engine::SIMULATION_TICKS_PER_SECOND + rate - 1) / rate;

    runTicks(rtype::engine::SIMULATION_TICKS_PER_SECOND);

    const std::vector<std::uint32_t> ticks =
        ticksSentTo(sentDatagrams(), client(FIRST_CLIENT_PORT));
    for (std::size_t index = 1; index < ticks.size(); ++index) {
      EXPECT_LE(ticks[index] - ticks[index - 1], longestGapInTicks);
    }
  }
}

/**
 * Given a sender
 * When it is built for a rate outside the lowest to the highest the server
 * accepts
 * Then it refuses it
 */
TEST_F(WorldStateSenderFixture, RefusesARateTheServerDoesNotAccept) {
  for (const std::size_t rate : REFUSED_STATE_RATES) {
    SCOPED_TRACE(rate);

    EXPECT_THROW(restartWithStatesPerSecond(rate),
                 rtype::server::InvalidStateRateException);
  }
}
