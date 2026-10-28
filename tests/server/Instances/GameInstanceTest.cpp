#include <gtest/gtest.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "DebugJournal.hpp"
#include "EnemyKilled.hpp"
#include "EntityDestruction.hpp"
#include "EventBus.hpp"
#include "GameInstance.hpp"
#include "GameInstanceConstants.hpp"
#include "GameInstanceFixtures.hpp"
#include "GameInstanceTestConstants.hpp"
#include "GameState.hpp"
#include "IncomingDatagram.hpp"
#include "LobbyState.hpp"
#include "MessageType.hpp"
#include "NetworkTestConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerInput.hpp"
#include "PlayerLeft.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "Replication/GameEventTestConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerConstants.hpp"
#include "ServerDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "TimeConstants.hpp"
#include "TimerScheduler.hpp"
#include "UnreliableNetwork.hpp"
#include "WorldState.hpp"
#include "game/ShipControl/ShipControlTestConstants.hpp"

namespace rtype::server {

using game::testing::FIRST_TICK_DISTANCE;
using game::testing::HELD_SECOND_DISTANCE;
using game::testing::POSITION_TOLERANCE;
using game::testing::ROLL_DISTANCE;
using game::testing::ROLL_END_TICK;
using testing::bydosAmong;
using testing::bydosIn;
using testing::entityOf;
using testing::FIRST_PLAYER;
using testing::FIRST_SENT;
using testing::FIRST_SEQUENCE;
using testing::FOURTH_PLAYER;
using testing::FOURTH_SENT;
using testing::FOURTH_SEQUENCE;
using testing::GameInstanceArrowTest;
using testing::GameInstanceInputShuffleTest;
using testing::GameInstanceInputTest;
using testing::GameInstanceLogTest;
using testing::GameInstanceMovementTest;
using testing::GameInstanceTest;
using testing::HeldArrow;
using testing::HOLDS_DOWN;
using testing::HOLDS_LEFT;
using testing::HOLDS_NOTHING;
using testing::HOLDS_RIGHT;
using testing::HOLDS_SHOOT;
using testing::HOLDS_UP;
using testing::inputPayload;
using testing::LARGEST_INPUT_NUMBER;
using testing::messageTypesOf;
using testing::NEXT_PROTOCOL_VERSION;
using testing::ofType;
using testing::onlyMessageIn;
using testing::PLAYER_SESSION_TOKENS;
using testing::playerEndpoint;
using testing::PROTOCOL_HEARTBEAT_TICKS;
using testing::PROTOCOL_SESSION_TIMEOUT_TICKS;
using testing::roomOfTheFirstPlayerNotReady;
using testing::SECOND_PLAYER;
using testing::SECOND_SENT;
using testing::SECOND_SEQUENCE;
using testing::sessionHeader;
using testing::SHUFFLE_SEED_COUNT;
using testing::SHUFFLED_INPUT_COUNT;
using testing::SOME_POINTS;
using testing::SOME_TICKS;
using testing::STRANGER;
using testing::THIRD_PLAYER;
using testing::THIRD_SENT;
using testing::THIRD_SEQUENCE;
using testing::TICKS_PER_REJECTION_LINE;
using testing::UNLISTED_ACTION;
using testing::WIRE_COORDINATE_TOLERANCE;

/**
 * Given four connected players of whom the first three are ready
 * When the fourth says ready
 * Then the game starts on that tick and not before
 */
TEST_F(GameInstanceTest, StartsOnlyOnceTheLastPlayerIsReady) {
  for (std::size_t index = 0; index < network::MAXIMUM_PLAYERS; ++index) {
    join(index);
  }
  for (std::size_t index = 0; index < FOURTH_PLAYER; ++index) {
    sendReady({.player = index, .ready = true, .sequence = FIRST_SEQUENCE});
  }
  tick();
  ASSERT_EQ(game().state(), GameState::WaitingForPlayers);
  sendReady(
      {.player = FOURTH_PLAYER, .ready = true, .sequence = FIRST_SEQUENCE});

  tick();

  EXPECT_EQ(game().state(), GameState::InProgress);
}

/**
 * Given a player who said ready
 * When the player says not ready with a newer sequence
 * Then the player is no longer ready and the game keeps waiting
 */
TEST_F(GameInstanceTest, CancelsReadinessWhenThePlayerSaysNotReady) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);
  join(SECOND_PLAYER);
  sendReady(
      {.player = FIRST_PLAYER, .ready = true, .sequence = FIRST_SEQUENCE});
  tick();
  sendReady(
      {.player = FIRST_PLAYER, .ready = false, .sequence = SECOND_SEQUENCE});

  tick();

  EXPECT_FALSE(player.ready);
}

/**
 * Given a connected player
 * When a ReadyState arrives from an address that is not connected
 * Then the player stays not ready and the game keeps waiting
 */
TEST_F(GameInstanceTest, IgnoresAReadyStateFromAnUnknownSender) {
  join(FIRST_PLAYER);
  sendReady(
      {.player = SECOND_PLAYER, .ready = true, .sequence = FIRST_SEQUENCE});

  tick();

  EXPECT_EQ(game().state(), GameState::WaitingForPlayers);
}

/**
 * Given a player who said ready with sequence 3
 * When a late not ready with sequence 2 arrives
 * Then the player is still ready
 */
TEST_F(GameInstanceTest, IgnoresAReadyStateOlderThanTheNewestAccepted) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);
  join(SECOND_PLAYER);
  sendReady(
      {.player = FIRST_PLAYER, .ready = true, .sequence = THIRD_SEQUENCE});
  tick();
  sendReady(
      {.player = FIRST_PLAYER, .ready = false, .sequence = SECOND_SEQUENCE});

  tick();

  EXPECT_TRUE(player.ready);
}

/**
 * Given a player who said ready with sequence 3
 * When the same datagram arrives again
 * Then the player is still ready and the sequence is unchanged
 */
TEST_F(GameInstanceTest, IgnoresADuplicateReadyState) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);
  join(SECOND_PLAYER);
  sendReady(
      {.player = FIRST_PLAYER, .ready = true, .sequence = THIRD_SEQUENCE});
  tick();
  sendReady(
      {.player = FIRST_PLAYER, .ready = false, .sequence = THIRD_SEQUENCE});

  tick();

  EXPECT_TRUE(player.ready);
}

/**
 * Given a game that waits for players
 * When a datagram of another protocol arrives
 * Then the tick does not throw and the game keeps waiting
 */
TEST_F(GameInstanceTest, IgnoresADatagramThatIsNotOfTheProtocol) {
  join(FIRST_PLAYER);
  queue(network::IncomingDatagram{
      .sender = playerEndpoint(FIRST_PLAYER),
      .payload = std::vector<std::byte>(network::PACKET_HEADER_SIZE)});

  EXPECT_NO_THROW(tick());
  EXPECT_EQ(game().state(), GameState::WaitingForPlayers);
}

/**
 * Given a game in progress
 * When a player sends not ready
 * Then the game stays in progress and the player stays ready
 */
TEST_F(GameInstanceTest, IgnoresAReadyStateWhileInProgress) {
  startGameWithOnePlayer();
  sendReady(
      {.player = FIRST_PLAYER, .ready = false, .sequence = SECOND_SEQUENCE});

  tick();

  EXPECT_EQ(game().state(), GameState::InProgress);
  EXPECT_TRUE(game().roster().find(playerEndpoint(FIRST_PLAYER))->ready);
}

/**
 * Given a game in progress with one player
 * When that player times out
 * Then the game is finished
 */
TEST_F(GameInstanceTest, FinishesTheGameInProgressItsLastPlayerLeft) {
  startGameWithOnePlayer();

  timeOut(FIRST_PLAYER);

  EXPECT_EQ(game().state(), GameState::Finished);
}

/**
 * Given a game in progress with one player
 * When that player sends a Disconnection
 * Then the game is finished in the tick that reads it, without waiting for
 * the timeout
 */
TEST_F(GameInstanceTest, FinishesTheGameInTheTickItsLastPlayerLeaves) {
  startGameWithOnePlayer();

  leave(FIRST_PLAYER);
  tick();

  EXPECT_EQ(game().state(), GameState::Finished);
}

/**
 * Given a game in progress with two players
 * When one of them times out
 * Then the game goes on for the other
 */
TEST_F(GameInstanceTest, PlaysOnWhileOnePlayerIsLeft) {
  startGameWith({FIRST_PLAYER, SECOND_PLAYER});

  timeOut(SECOND_PLAYER);

  EXPECT_EQ(game().state(), GameState::InProgress);
}

/**
 * Given a game in progress whose last player timed out
 * When the finished duration elapses
 * Then the game waits for players again, so a new player may join it
 */
TEST_F(GameInstanceTest, WaitsForPlayersAgainAfterItsLastPlayerLeft) {
  startGameWithOnePlayer();
  timeOut(FIRST_PLAYER);

  runTicks(FINISHED_STATE_TICKS);

  EXPECT_EQ(game().state(), GameState::WaitingForPlayers);
}

/**
 * Given a game in progress whose player killed an enemy and a game that
 * finished
 * When the game waits for players again
 * Then the player's score is back to 0
 */
TEST_F(GameInstanceTest, ResetsTheScoreBetweenTwoGames) {
  startGameWithOnePlayer();
  const ConnectedPlayer* const player =
      game().roster().findByNumber(network::FIRST_PLAYER_NUMBER);
  ASSERT_NE(player, nullptr);
  events().publish(game::EnemyKilled{
      .playerNumber = network::FIRST_PLAYER_NUMBER, .points = SOME_POINTS});
  events().dispatch();
  ASSERT_EQ(player->score, SOME_POINTS);
  game().finish();

  runTicks(FINISHED_STATE_TICKS);

  ASSERT_EQ(game().roster().connected().size(), 1U);
  EXPECT_EQ(player->score, 0U);
}

/**
 * Given a game in progress
 * When an enemy is killed by the first player
 * Then the score of that player grows by the points of the enemy
 */
TEST_F(GameInstanceTest, CreditsTheKillOfAnEnemyToThePlayerWhoFired) {
  startGameWithOnePlayer();
  const ConnectedPlayer* const player =
      game().roster().findByNumber(network::FIRST_PLAYER_NUMBER);
  ASSERT_NE(player, nullptr);

  events().publish(game::EnemyKilled{
      .playerNumber = network::FIRST_PLAYER_NUMBER, .points = SOME_POINTS});
  events().dispatch();

  EXPECT_EQ(player->score, SOME_POINTS);
}

/**
 * Given a game that waits for players again after a game
 * When the player says ready with a newer sequence
 * Then the second game starts
 */
TEST_F(GameInstanceTest, StartsASecondGame) {
  startGameWithOnePlayer();
  game().finish();
  runTicks(FINISHED_STATE_TICKS);
  sendReady(
      {.player = FIRST_PLAYER, .ready = true, .sequence = FOURTH_SEQUENCE});

  tick();

  EXPECT_EQ(game().state(), GameState::InProgress);
}

/**
 * Given a game in progress for 18 seconds: the three Bydos of its first wave
 * appear 4, 6 and 8 seconds in and take about 8.5 seconds to cross the
 * playfield, and the next wave comes a pause of 4 seconds after the last left
 * When the events are dispatched
 * Then the three Bydos are gone, a removal was announced for each, and the
 * world holds no Bydo; the missiles the Bydos fire leave on the left too,
 * and are not counted
 */
TEST_F(GameInstanceTest, ABydoThatLeavesOnTheLeftDoesNotStayInTheWorld) {
  std::vector<std::uint32_t> removed;
  events().subscribe<game::EntityDestruction>(
      [&removed](const game::EntityDestruction& event) {
        if (event.cause == game::DestructionCause::Removed) {
          removed.push_back(event.networkId);
        }
      });
  startGameWithOnePlayer();

  runTicks(testing::FIRST_WAVE_GONE_TICKS);
  events().dispatch();

  EXPECT_EQ(bydosAmong(removed, sentTo(FIRST_PLAYER)), 3U);
  EXPECT_EQ(bydosIn(game().world()), 0U);
}

/**
 * Given a game in progress whose first wave left the playfield, its last Bydo
 * about 16.6 seconds in
 * When the game reaches 21.5 seconds and the events are dispatched
 * Then the three Bydos of the first wave were removed, and the world holds the
 * first Bydo of the second wave only, come a pause after the first wave left;
 * the missiles the Bydos fire are not counted
 */
TEST_F(GameInstanceTest, TheSecondWaveComesAPauseAfterTheFirstIsGone) {
  std::vector<std::uint32_t> removed;
  events().subscribe<game::EntityDestruction>(
      [&removed](const game::EntityDestruction& event) {
        if (event.cause == game::DestructionCause::Removed) {
          removed.push_back(event.networkId);
        }
      });
  startGameWithOnePlayer();

  runTicks(testing::SECOND_WAVE_BEGUN_TICKS);
  events().dispatch();

  EXPECT_EQ(bydosAmong(removed, sentTo(FIRST_PLAYER)), 3U);
  EXPECT_EQ(bydosIn(game().world()), 1U);
}

/**
 * Given one connected player, sent the EntitySpawned of its ship
 * When the header of the next datagram to its address is asked for
 * Then it carries the session token of the player and its next sequence, 1,
 * which then moves on by one
 */
TEST_F(GameInstanceTest, GivesTheNextSessionHeaderOfAPlayer) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);

  const network::PacketHeader header =
      game().nextSessionHeaderFor(playerEndpoint(FIRST_PLAYER));

  EXPECT_EQ(header.sessionToken, testing::SESSION_TOKEN);
  EXPECT_EQ(header.sequence, 1);
  EXPECT_EQ(player.nextSequence, 2);
}

/**
 * Given one connected player who is not ready
 * When a tick runs
 * Then the player is sent the room, which lists it, not ready, under its
 * nickname
 */
TEST_F(GameInstanceTest, SendsTheRoomToAWaitingPlayer) {
  join(FIRST_PLAYER);

  tick();

  const std::vector<network::OutgoingDatagram> rooms =
      ofType(network::MessageType::LobbyState, sentTo(FIRST_PLAYER));
  ASSERT_EQ(rooms.size(), 1U);
  EXPECT_EQ(onlyMessageIn<network::LobbyState>(rooms.front()),
            roomOfTheFirstPlayerNotReady());
}

/**
 * Given one connected player, sent the EntitySpawned of its ship
 * When the player says ready and the tick runs
 * Then the player is sent GameStarted, then a WorldState
 */
TEST_F(GameInstanceTest, SendsGameStartedBeforeTheFirstWorldState) {
  join(FIRST_PLAYER);
  sendReady(
      {.player = FIRST_PLAYER, .ready = true, .sequence = FIRST_SEQUENCE});

  tick();

  EXPECT_EQ(messageTypesOf(sentTo(FIRST_PLAYER)),
            (std::vector{network::MessageType::EntitySpawned,
                         network::MessageType::GameStarted,
                         network::MessageType::WorldState}));
}

/**
 * Given a game just started by its only player
 * When TICKS_PER_WORLD_STATE more ticks run
 * Then the player was sent the WorldState of tick 0 and of tick
 * TICKS_PER_WORLD_STATE, each starting with a header DatagramCodec reads,
 * carrying its session token
 */
TEST_F(GameInstanceTest, SendsTheWorldStatesUnderTheTokenOfThePlayer) {
  startGameWithOnePlayer();

  runTicks(TICKS_PER_WORLD_STATE);

  const std::vector<network::OutgoingDatagram> states =
      ofType(network::MessageType::WorldState, sentTo(FIRST_PLAYER));
  ASSERT_EQ(states.size(), 2U);
  EXPECT_EQ(onlyMessageIn<network::WorldState>(states.at(0)).tick, 0U);
  EXPECT_EQ(onlyMessageIn<network::WorldState>(states.at(1)).tick,
            TICKS_PER_WORLD_STATE);
  for (const network::OutgoingDatagram& state : states) {
    EXPECT_EQ(network::DatagramCodec::decodeHeader(state.payload).sessionToken,
              testing::SESSION_TOKEN);
  }
}

/**
 * Given one connected player, sent the EntitySpawned of its ship
 * When it waits one tick, says ready, and the game runs until its second
 * WorldState
 * Then the datagrams it was sent, whatever their message, carry the sequences
 * 0, 1, 2… in the order they were sent, so the client reads every one
 */
TEST_F(GameInstanceTest, NumbersEveryDatagramToAPlayerFromOneCounter) {
  join(FIRST_PLAYER);
  tick();
  sendReady(
      {.player = FIRST_PLAYER, .ready = true, .sequence = FIRST_SEQUENCE});
  tick();
  runTicks(TICKS_PER_WORLD_STATE);

  const std::vector<network::OutgoingDatagram> toPlayer = sentTo(FIRST_PLAYER);
  ASSERT_EQ(
      messageTypesOf(toPlayer),
      (std::vector{
          network::MessageType::EntitySpawned, network::MessageType::LobbyState,
          network::MessageType::GameStarted, network::MessageType::WorldState,
          network::MessageType::WorldState}));
  for (std::size_t index = 0; index < toPlayer.size(); ++index) {
    EXPECT_EQ(network::DatagramCodec::decodeHeader(toPlayer.at(index).payload)
                  .sequence,
              static_cast<std::uint16_t>(index));
  }
}

/**
 * Given two connected players with their own session tokens
 * When both say ready and the tick runs
 * Then each is sent one GameStarted, under its own token
 */
TEST_F(GameInstanceTest, TellsEveryPlayerThatTheGameStarted) {
  join(FIRST_PLAYER, testing::SESSION_TOKEN);
  join(SECOND_PLAYER, testing::OTHER_SESSION_TOKEN);
  sendReady({.player = FIRST_PLAYER,
             .ready = true,
             .sequence = FIRST_SEQUENCE,
             .sessionToken = testing::SESSION_TOKEN});
  sendReady({.player = SECOND_PLAYER,
             .ready = true,
             .sequence = FIRST_SEQUENCE,
             .sessionToken = testing::OTHER_SESSION_TOKEN});

  tick();

  const std::vector<network::OutgoingDatagram> toFirst =
      ofType(network::MessageType::GameStarted, sentTo(FIRST_PLAYER));
  const std::vector<network::OutgoingDatagram> toSecond =
      ofType(network::MessageType::GameStarted, sentTo(SECOND_PLAYER));
  ASSERT_EQ(toFirst.size(), 1U);
  ASSERT_EQ(toSecond.size(), 1U);
  EXPECT_EQ(network::DatagramCodec::decodeHeader(toFirst.front().payload)
                .sessionToken,
            testing::SESSION_TOKEN);
  EXPECT_EQ(network::DatagramCodec::decodeHeader(toSecond.front().payload)
                .sessionToken,
            testing::OTHER_SESSION_TOKEN);
}

/**
 * Given two connected players
 * When only the first says ready and the tick runs
 * Then neither is sent GameStarted or a WorldState
 */
TEST_F(GameInstanceTest, AnnouncesNoStartWhileAPlayerIsNotReady) {
  join(FIRST_PLAYER);
  join(SECOND_PLAYER);
  sendReady(
      {.player = FIRST_PLAYER, .ready = true, .sequence = FIRST_SEQUENCE});

  tick();

  for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
    const std::vector<network::OutgoingDatagram> toPlayer = sentTo(player);
    EXPECT_TRUE(ofType(network::MessageType::GameStarted, toPlayer).empty());
    EXPECT_TRUE(ofType(network::MessageType::WorldState, toPlayer).empty());
  }
}

/**
 * Given one connected player who is not ready
 * When TICKS_PER_WORLD_STATE ticks run
 * Then the player is sent no WorldState
 */
TEST_F(GameInstanceTest, SendsNoWorldStateWhileWaiting) {
  join(FIRST_PLAYER);

  runTicks(TICKS_PER_WORLD_STATE);

  EXPECT_TRUE(
      ofType(network::MessageType::WorldState, sentTo(FIRST_PLAYER)).empty());
}

/**
 * Given a game just started by its only player
 * When more ticks run than between two repeats of the room
 * Then the player is sent no room during the game
 */
TEST_F(GameInstanceTest, SendsNoRoomDuringTheGame) {
  startGameWithOnePlayer();

  runTicks(TICKS_PER_LOBBY_STATE + 1);

  EXPECT_TRUE(
      ofType(network::MessageType::LobbyState, sentTo(FIRST_PLAYER)).empty());
}

/**
 * Given a player who was sent the room, then played a game that finished and
 * ran its finished duration, so the game waits for players again
 * When the first tick of the new wait runs
 * Then the player is sent the room at once, showing it not ready
 */
TEST_F(GameInstanceTest, SendsTheRoomAtOnceWhenTheGameWaitsAgain) {
  join(FIRST_PLAYER);
  tick();
  sendReady(
      {.player = FIRST_PLAYER, .ready = true, .sequence = FIRST_SEQUENCE});
  tick();
  game().finish();
  runTicks(FINISHED_STATE_TICKS);
  ASSERT_EQ(game().state(), GameState::WaitingForPlayers);
  const std::size_t roomsBefore =
      ofType(network::MessageType::LobbyState, sentTo(FIRST_PLAYER)).size();

  tick();

  const std::vector<network::OutgoingDatagram> rooms =
      ofType(network::MessageType::LobbyState, sentTo(FIRST_PLAYER));
  ASSERT_EQ(rooms.size(), roomsBefore + 1);
  EXPECT_EQ(onlyMessageIn<network::LobbyState>(rooms.back()),
            roomOfTheFirstPlayerNotReady());
}

/**
 * Given a player heard from in a tick, then silent
 * When the timeout of the protocol, less one tick, runs without a datagram
 * Then the player is still in the game
 */
TEST_F(GameInstanceTest, KeepsAPlayerSilentForOneTickLessThanTheTimeout) {
  join(FIRST_PLAYER);
  tick();
  silence(FIRST_PLAYER);

  runTicks(PROTOCOL_SESSION_TIMEOUT_TICKS - 1);

  EXPECT_NE(game().roster().find(playerEndpoint(FIRST_PLAYER)), nullptr);
}

/**
 * Given a player heard from in a tick, then silent
 * When the timeout of the protocol, 5 s of ticks, runs without a datagram
 * Then the player is no longer in the game
 */
TEST_F(GameInstanceTest, ClosesTheSessionOfAPlayerSilentForTheTimeout) {
  join(FIRST_PLAYER);
  tick();
  silence(FIRST_PLAYER);

  runTicks(PROTOCOL_SESSION_TIMEOUT_TICKS);

  EXPECT_EQ(game().roster().find(playerEndpoint(FIRST_PLAYER)), nullptr);
}

/**
 * Given a player whose client sends a Heartbeat before every tick
 * When twice the timeout runs
 * Then the player is still in the game
 */
TEST_F(GameInstanceTest, KeepsAPlayerWhoSendsHeartbeats) {
  join(FIRST_PLAYER);

  runTicks(2 * PROTOCOL_SESSION_TIMEOUT_TICKS);

  EXPECT_NE(game().roster().find(playerEndpoint(FIRST_PLAYER)), nullptr);
}

/**
 * Given a player silent for the timeout less one tick
 * When it sends a ReadyState, then stays silent for the timeout less one tick
 * Then it is still in the game: any datagram it sends starts the delay over
 */
TEST_F(GameInstanceTest, AnyDatagramOfAPlayerStartsTheTimeoutOver) {
  join(FIRST_PLAYER);
  join(SECOND_PLAYER);
  tick();
  silence(FIRST_PLAYER);
  runTicks(PROTOCOL_SESSION_TIMEOUT_TICKS - 1);
  sendReady(
      {.player = FIRST_PLAYER, .ready = true, .sequence = FIRST_SEQUENCE});
  tick();

  runTicks(PROTOCOL_SESSION_TIMEOUT_TICKS - 1);

  EXPECT_NE(game().roster().find(playerEndpoint(FIRST_PLAYER)), nullptr);
}

/**
 * Given two players, the second of whom goes silent
 * When the timeout runs
 * Then the ship of the second player leaves the world, announced as removed
 * without an explosion, and the ship of the first stays
 */
TEST_F(GameInstanceTest, RemovesTheShipOfAPlayerWhoTimedOut) {
  join(FIRST_PLAYER);
  const std::uint32_t ship = join(SECOND_PLAYER).shipEntityId;
  std::vector<game::EntityDestruction> destructions;
  events().subscribe<game::EntityDestruction>(
      [&destructions](const game::EntityDestruction& event) {
        destructions.push_back(event);
      });

  timeOut(SECOND_PLAYER);
  events().dispatch();

  EXPECT_EQ(destructions,
            (std::vector{game::EntityDestruction{
                .networkId = ship, .cause = game::DestructionCause::Removed}}));
  EXPECT_EQ(game().world().entityCount(), 1U);
}

/**
 * Given two players, the second of whom goes silent
 * When the timeout runs
 * Then the departure of player 2 is announced once, as timed out
 */
TEST_F(GameInstanceTest, AnnouncesThePlayerWhoTimedOut) {
  join(FIRST_PLAYER);
  join(SECOND_PLAYER);
  std::vector<game::PlayerDeparture> departures;
  events().subscribe<game::PlayerDeparture>(
      [&departures](const game::PlayerDeparture& event) {
        departures.push_back(event);
      });
  silence(SECOND_PLAYER);

  runTicks(2 * PROTOCOL_SESSION_TIMEOUT_TICKS);
  events().dispatch();

  EXPECT_EQ(departures, (std::vector{game::PlayerDeparture{
                            .playerNumber = network::FIRST_PLAYER_NUMBER + 1,
                            .cause = game::DepartureCause::TimedOut}}));
}

/**
 * Given two players, the second of whom timed out
 * When the departed players are taken twice
 * Then the first take holds the second player, with its address and token,
 * and the second take holds nobody
 */
TEST_F(GameInstanceTest, HandsOverThePlayerWhoTimedOutOnce) {
  join(FIRST_PLAYER);
  join(SECOND_PLAYER, testing::OTHER_SESSION_TOKEN);
  timeOut(SECOND_PLAYER);

  const std::vector<ConnectedPlayer> first = game().takeDepartedPlayers();
  const std::vector<ConnectedPlayer> second = game().takeDepartedPlayers();

  ASSERT_EQ(first.size(), 1U);
  EXPECT_EQ(first.front().endpoint, playerEndpoint(SECOND_PLAYER));
  EXPECT_EQ(first.front().sessionToken, testing::OTHER_SESSION_TOKEN);
  EXPECT_TRUE(second.empty());
}

/**
 * Given three players in a game in progress, the second of whom goes silent
 * When the timeout runs, then one more world state goes out
 * Then the game goes on, and that state reaches the two others without the
 * ship of the second player
 */
TEST_F(GameInstanceTest, TheOthersPlayOnWhenOneOfThreePlayersTimesOut) {
  startGameWith({FIRST_PLAYER, SECOND_PLAYER, THIRD_PLAYER});
  const std::vector<std::uint32_t> ships{
      shipOf(FIRST_PLAYER), shipOf(SECOND_PLAYER), shipOf(THIRD_PLAYER)};
  timeOut(SECOND_PLAYER);
  const std::size_t sentToTheFirst =
      ofType(network::MessageType::WorldState, sentTo(FIRST_PLAYER)).size();

  runTicks(TICKS_PER_WORLD_STATE);

  EXPECT_EQ(game().state(), GameState::InProgress);
  for (const std::size_t player : {FIRST_PLAYER, THIRD_PLAYER}) {
    const std::vector<network::OutgoingDatagram> states =
        ofType(network::MessageType::WorldState, sentTo(player));
    ASSERT_EQ(states.size(), sentToTheFirst + 1);
    std::vector<std::uint32_t> listed;
    for (const network::EntityState& entity :
         onlyMessageIn<network::WorldState>(states.back()).entities) {
      listed.push_back(entity.entityId);
    }
    EXPECT_EQ(std::ranges::count(listed, ships.at(FIRST_PLAYER)), 1);
    EXPECT_EQ(std::ranges::count(listed, ships.at(THIRD_PLAYER)), 1);
    EXPECT_EQ(std::ranges::count(listed, ships.at(SECOND_PLAYER)), 0);
  }
}

/**
 * Given three players, the second of whom goes silent, and the broadcaster of
 * the game events wired to the roster as the server wires it
 * When the timeout runs, the bus delivers the events and every copy comes due
 * Then the first and the third player get three PlayerLeft naming player 2,
 * timed out, and the second player gets none
 */
TEST_F(GameInstanceTest, TellsOnlyTheOthersThatAPlayerTimedOut) {
  for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER, THIRD_PLAYER}) {
    join(player);
  }

  timeOut(SECOND_PLAYER);
  events().dispatch();
  timers().advance(testing::ALL_COPIES_SENT);

  const std::vector expected(
      testing::PROTOCOL_REPEATED_MESSAGE_COPIES,
      network::PlayerLeft{.playerNumber = network::FIRST_PLAYER_NUMBER + 1,
                          .reason = network::DepartureReason::TimedOut});
  EXPECT_EQ(playerLeftSentTo(sentDatagrams(), FIRST_PLAYER), expected);
  EXPECT_EQ(playerLeftSentTo(sentDatagrams(), THIRD_PLAYER), expected);
  EXPECT_TRUE(playerLeftSentTo(sentDatagrams(), SECOND_PLAYER).empty());
}

/**
 * Given three players, and the broadcaster of the game events wired to the
 * roster as the server wires it
 * When the second sends a Disconnection, the bus delivers the events and every
 * copy comes due
 * Then the first and the third player get three PlayerLeft naming player 2,
 * left, and the second player gets none
 */
TEST_F(GameInstanceTest, TellsOnlyTheOthersThatAPlayerLeft) {
  for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER, THIRD_PLAYER}) {
    join(player);
  }

  leave(SECOND_PLAYER);
  tick();
  events().dispatch();
  timers().advance(testing::ALL_COPIES_SENT);

  const std::vector expected(
      testing::PROTOCOL_REPEATED_MESSAGE_COPIES,
      network::PlayerLeft{.playerNumber = network::FIRST_PLAYER_NUMBER + 1,
                          .reason = network::DepartureReason::Left});
  EXPECT_EQ(playerLeftSentTo(sentDatagrams(), FIRST_PLAYER), expected);
  EXPECT_EQ(playerLeftSentTo(sentDatagrams(), THIRD_PLAYER), expected);
  EXPECT_TRUE(playerLeftSentTo(sentDatagrams(), SECOND_PLAYER).empty());
}

/**
 * Given three players in a game in progress, the second of whom timed out
 * When more world states go out
 * Then none reaches the second player
 */
TEST_F(GameInstanceTest, SendsNothingToAPlayerWhoTimedOut) {
  startGameWith({FIRST_PLAYER, SECOND_PLAYER, THIRD_PLAYER});
  timeOut(SECOND_PLAYER);
  const std::size_t sentBefore = sentTo(SECOND_PLAYER).size();

  runTicks(PROTOCOL_SESSION_TIMEOUT_TICKS);

  EXPECT_EQ(sentTo(SECOND_PLAYER).size(), sentBefore);
}

/**
 * Given two players, the second of whom timed out
 * When a client joins from another address
 * Then it takes the number the second player had, 2
 */
TEST_F(GameInstanceTest, GivesTheNumberOfAPlayerWhoTimedOutToTheNextOne) {
  join(FIRST_PLAYER);
  join(SECOND_PLAYER);
  timeOut(SECOND_PLAYER);

  const ConnectedPlayer& next = join(THIRD_PLAYER);

  EXPECT_EQ(next.playerNumber, network::FIRST_PLAYER_NUMBER + 1);
}

/**
 * Given two players
 * When the second sends a Disconnection
 * Then it is no longer in the game once that tick ran, and the first one is
 */
TEST_F(GameInstanceTest, ClosesAtOnceTheSessionOfThePlayerWhoLeaves) {
  join(FIRST_PLAYER);
  join(SECOND_PLAYER);

  leave(SECOND_PLAYER);
  tick();

  EXPECT_EQ(game().roster().find(playerEndpoint(SECOND_PLAYER)), nullptr);
  EXPECT_NE(game().roster().find(playerEndpoint(FIRST_PLAYER)), nullptr);
}

/**
 * Given two players
 * When the second sends a Disconnection
 * Then the ship of the second player leaves the world, announced as removed
 * without an explosion, and the ship of the first stays
 */
TEST_F(GameInstanceTest, RemovesTheShipOfAPlayerWhoLeaves) {
  join(FIRST_PLAYER);
  const std::uint32_t ship = join(SECOND_PLAYER).shipEntityId;
  std::vector<game::EntityDestruction> destructions;
  events().subscribe<game::EntityDestruction>(
      [&destructions](const game::EntityDestruction& event) {
        destructions.push_back(event);
      });

  leave(SECOND_PLAYER);
  tick();
  events().dispatch();

  EXPECT_EQ(destructions,
            (std::vector{game::EntityDestruction{
                .networkId = ship, .cause = game::DestructionCause::Removed}}));
  EXPECT_EQ(game().world().entityCount(), 1U);
}

/**
 * Given two players, the second of whom sends a Disconnection
 * When its two later copies arrive, one in the same tick and one in the next,
 * and the timeout runs
 * Then the departure of player 2 is announced once, as left
 */
TEST_F(GameInstanceTest, AnnouncesOnceThatAPlayerLeft) {
  join(FIRST_PLAYER);
  join(SECOND_PLAYER);
  std::vector<game::PlayerDeparture> departures;
  events().subscribe<game::PlayerDeparture>(
      [&departures](const game::PlayerDeparture& event) {
        departures.push_back(event);
      });

  leave(SECOND_PLAYER);
  queueDisconnectionFrom(SECOND_PLAYER);
  tick();
  queueDisconnectionFrom(SECOND_PLAYER);
  tick();
  runTicks(PROTOCOL_SESSION_TIMEOUT_TICKS + 1);
  events().dispatch();

  EXPECT_EQ(departures, (std::vector{game::PlayerDeparture{
                            .playerNumber = network::FIRST_PLAYER_NUMBER + 1,
                            .cause = game::DepartureCause::Left}}));
}

/**
 * Given two players, the second of whom sent a Disconnection
 * When the departed players are taken twice
 * Then the first take holds the second player, with its address and token,
 * and the second take holds nobody
 */
TEST_F(GameInstanceTest, HandsOverThePlayerWhoLeftOnce) {
  join(FIRST_PLAYER);
  join(SECOND_PLAYER, testing::OTHER_SESSION_TOKEN);
  leave(SECOND_PLAYER);
  tick();

  const std::vector<ConnectedPlayer> first = game().takeDepartedPlayers();
  const std::vector<ConnectedPlayer> second = game().takeDepartedPlayers();

  ASSERT_EQ(first.size(), 1U);
  EXPECT_EQ(first.front().endpoint, playerEndpoint(SECOND_PLAYER));
  EXPECT_EQ(first.front().sessionToken, testing::OTHER_SESSION_TOKEN);
  EXPECT_TRUE(second.empty());
}

/**
 * Given three players in a game in progress
 * When the second sends a Disconnection
 * Then the game goes on for the two others
 */
TEST_F(GameInstanceTest, TheOthersPlayOnWhenAPlayerLeaves) {
  startGameWith({FIRST_PLAYER, SECOND_PLAYER, THIRD_PLAYER});

  leave(SECOND_PLAYER);
  tick();

  EXPECT_EQ(game().state(), GameState::InProgress);
  EXPECT_EQ(game().roster().connected().size(), 2U);
}

/**
 * Given a game that one player started, then finished, which sends that
 * player nothing while it stays finished
 * When one second of ticks less one runs
 * Then the player is sent no Heartbeat yet
 */
TEST_F(GameInstanceTest, WaitsASecondWithoutDatagramBeforeAHeartbeat) {
  startGameWithOnePlayer();
  game().finish();

  runTicks(PROTOCOL_HEARTBEAT_TICKS - 1);

  EXPECT_TRUE(
      ofType(network::MessageType::Heartbeat, sentTo(FIRST_PLAYER)).empty());
}

/**
 * Given a game that one player started, then finished, which sends that
 * player nothing while it stays finished
 * When one second of ticks runs
 * Then the player is sent one Heartbeat, so its client knows the server is
 * still there
 */
TEST_F(GameInstanceTest, SendsAHeartbeatAfterASecondWithoutDatagram) {
  startGameWithOnePlayer();
  game().finish();

  runTicks(PROTOCOL_HEARTBEAT_TICKS);

  EXPECT_EQ(
      ofType(network::MessageType::Heartbeat, sentTo(FIRST_PLAYER)).size(), 1U);
}

/**
 * Given a finished game, which sends its player nothing else
 * When the Heartbeat goes out
 * Then it carries the session token of the player and the sequence after the
 * datagram sent before it
 */
TEST_F(GameInstanceTest, SendsTheHeartbeatUnderTheSessionOfThePlayer) {
  startGameWithOnePlayer();
  game().finish();
  const std::uint16_t sequenceBefore =
      network::DatagramCodec::decodeHeader(sentTo(FIRST_PLAYER).back().payload)
          .sequence;

  runTicks(PROTOCOL_HEARTBEAT_TICKS);

  const network::PacketHeader header =
      network::DatagramCodec::decodeHeader(sentTo(FIRST_PLAYER).back().payload);
  EXPECT_EQ(header.sessionToken, testing::SESSION_TOKEN);
  EXPECT_EQ(header.sequence, sequenceBefore + 1);
}

/**
 * Given a finished game, which sends its player nothing else
 * When three seconds of ticks run
 * Then the player is sent a Heartbeat every second, three in all
 */
TEST_F(GameInstanceTest, SendsAHeartbeatEverySecondWhileNothingElseGoesOut) {
  startGameWithOnePlayer();
  game().finish();

  runTicks(3 * PROTOCOL_HEARTBEAT_TICKS);

  EXPECT_EQ(
      ofType(network::MessageType::Heartbeat, sentTo(FIRST_PLAYER)).size(), 3U);
}

/**
 * Given a game in progress, which sends its player the world state 20 times
 * per second
 * When two seconds of ticks run
 * Then no Heartbeat is sent: the world states already prove the server is
 * there
 */
TEST_F(GameInstanceTest, SendsNoHeartbeatWhileTheWorldStateGoesOut) {
  startGameWithOnePlayer();

  runTicks(2 * PROTOCOL_HEARTBEAT_TICKS);

  EXPECT_TRUE(
      ofType(network::MessageType::Heartbeat, sentTo(FIRST_PLAYER)).empty());
}

/**
 * Given a waiting room, which sends its player the room every 500 ms
 * When two seconds of ticks run
 * Then no Heartbeat is sent
 */
TEST_F(GameInstanceTest, SendsNoHeartbeatWhileTheRoomGoesOut) {
  join(FIRST_PLAYER);

  runTicks(2 * PROTOCOL_HEARTBEAT_TICKS);

  EXPECT_TRUE(
      ofType(network::MessageType::Heartbeat, sentTo(FIRST_PLAYER)).empty());
}

/**
 * Given a waiting room where the first player is ready and the second, not
 * ready, goes silent
 * When the timeout runs
 * Then the game starts with the first player: every connected player is ready
 */
TEST_F(GameInstanceTest, StartsOnceThePlayerNotReadyTimedOut) {
  join(FIRST_PLAYER);
  join(SECOND_PLAYER);
  sendReady(
      {.player = FIRST_PLAYER, .ready = true, .sequence = FIRST_SEQUENCE});
  tick();
  ASSERT_EQ(game().state(), GameState::WaitingForPlayers);
  silence(SECOND_PLAYER);

  runTicks(PROTOCOL_SESSION_TIMEOUT_TICKS);

  EXPECT_EQ(game().state(), GameState::InProgress);
}

/**
 * Given a player whose datagram holds a ReadyState and no input
 * When a tick reads it
 * Then no input is kept for the player
 */
TEST_F(GameInstanceInputTest, KeepsNoInputFromADatagramWithoutOne) {
  sendDatagram(FIRST_PLAYER, readyDatagram(sessionHeader(FIRST_PLAYER)));

  deliver({FIRST_SENT});

  EXPECT_FALSE(newestInputOf(FIRST_PLAYER).has_value());
}

/**
 * Given a player who sent one input holding up
 * When a tick reads it
 * Then the player holds up
 */
TEST_F(GameInstanceInputTest, KeepsTheFirstInputOfAPlayer) {
  send(FIRST_PLAYER, HOLDS_UP);

  deliver({FIRST_SENT});

  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_UP);
}

/**
 * Given a player who sent up, then down, and whose up arrived in a tick
 * When down arrives in the next tick
 * Then the player holds down
 */
TEST_F(GameInstanceInputTest, KeepsANewerInputOfALaterTick) {
  send(FIRST_PLAYER, HOLDS_UP);
  send(FIRST_PLAYER, HOLDS_DOWN);
  deliver({FIRST_SENT});

  deliver({SECOND_SENT});

  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_DOWN);
}

/**
 * Given a player who sent up, then down, and whose down arrived first
 * When up arrives in the next tick, late
 * Then the player still holds down
 */
TEST_F(GameInstanceInputTest, IgnoresAnOlderInputArrivingInALaterTick) {
  send(FIRST_PLAYER, HOLDS_UP);
  send(FIRST_PLAYER, HOLDS_DOWN);
  deliver({SECOND_SENT});

  deliver({FIRST_SENT});

  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_DOWN);
}

/**
 * Given a player who sent up, down, then shoot
 * When one tick receives them as shoot, up, down
 * Then the player holds shoot, the last one sent, not down, the last one
 * received
 */
TEST_F(GameInstanceInputTest, KeepsTheHighestInputOfATickWhateverTheOrder) {
  send(FIRST_PLAYER, HOLDS_UP);
  send(FIRST_PLAYER, HOLDS_DOWN);
  send(FIRST_PLAYER, HOLDS_SHOOT);

  deliver({THIRD_SENT, FIRST_SENT, SECOND_SENT});

  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_SHOOT);
}

/**
 * Given a player who sent up, then down, both received
 * When a duplicate of up arrives in a later tick
 * Then the player still holds down
 */
TEST_F(GameInstanceInputTest, IgnoresALateDuplicate) {
  send(FIRST_PLAYER, HOLDS_UP);
  send(FIRST_PLAYER, HOLDS_DOWN);
  deliver({FIRST_SENT, SECOND_SENT});

  deliver({FIRST_SENT});

  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_DOWN);
}

/**
 * Given a player whose input holding up was kept
 * When another input with the same number arrives, holding down
 * Then the player still holds up: an input is replaced only by a higher number
 */
TEST_F(GameInstanceInputTest, IgnoresAnInputNumberedLikeTheOneKept) {
  send(FIRST_PLAYER, HOLDS_UP);
  sendDatagram(
      FIRST_PLAYER,
      inputDatagram(
          network::PlayerInput{.inputNumber = lastInputNumber(FIRST_PLAYER),
                               .actions = HOLDS_DOWN},
          sessionHeader(FIRST_PLAYER)));

  deliver({FIRST_SENT, SECOND_SENT});

  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_UP);
}

/**
 * Given a first player who sent three inputs, and a second player who sent
 * one holding left
 * When one tick receives the four
 * Then the second player holds left: the numbers of one player never count
 * for another
 */
TEST_F(GameInstanceInputTest, KeepsTheInputsOfEachPlayerApart) {
  send(FIRST_PLAYER, HOLDS_UP);
  send(FIRST_PLAYER, HOLDS_DOWN);
  send(FIRST_PLAYER, HOLDS_SHOOT);
  send(SECOND_PLAYER, HOLDS_LEFT);

  deliver({FIRST_SENT, SECOND_SENT, THIRD_SENT, FOURTH_SENT});

  EXPECT_EQ(heldActions(SECOND_PLAYER), HOLDS_LEFT);
}

/**
 * Given a sender that is not in the roster, using the address of the players
 * and the session token of the first player
 * When a tick reads its input
 * Then the first player has no input: inputs are kept for the sender, never
 * for the owner of the token
 */
TEST_F(GameInstanceInputTest, IgnoresAnInputFromASenderNotInTheRoster) {
  sendDatagram(
      STRANGER,
      inputDatagram(network::PlayerInput{.inputNumber = LARGEST_INPUT_NUMBER,
                                         .actions = HOLDS_DOWN},
                    sessionHeader(FIRST_PLAYER)));

  deliver({FIRST_SENT});

  EXPECT_FALSE(newestInputOf(FIRST_PLAYER).has_value());
}

/**
 * Given a player who sent an input numbered LARGEST_INPUT_NUMBER whose
 * actions hold a bit the protocol does not list, then an input holding up
 * When one tick receives both
 * Then the player holds up: the refused input was not kept
 */
TEST_F(GameInstanceInputTest, IgnoresAnInputItsReaderRefuses) {
  sendDatagram(
      FIRST_PLAYER,
      network::DatagramCodec::encode(
          sessionHeader(FIRST_PLAYER), network::MessageType::PlayerInput,
          inputPayload(network::PlayerInput{.inputNumber = LARGEST_INPUT_NUMBER,
                                            .actions = UNLISTED_ACTION})));
  send(FIRST_PLAYER, HOLDS_UP);

  deliver({FIRST_SENT, SECOND_SENT});

  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_UP);
}

/**
 * Given a player whose first datagram lost its last byte, then who sent up
 * When one tick receives both
 * Then nothing is thrown and the player holds up
 */
TEST_F(GameInstanceInputTest, ReadsTheDatagramAfterATruncatedOne) {
  std::vector<std::byte> truncated =
      nextInputDatagram(FIRST_PLAYER, HOLDS_DOWN, sessionHeader(FIRST_PLAYER));
  truncated.pop_back();
  sendDatagram(FIRST_PLAYER, std::move(truncated));
  send(FIRST_PLAYER, HOLDS_UP);

  EXPECT_NO_THROW(deliver({FIRST_SENT, SECOND_SENT}));
  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_UP);
}

/**
 * Given a datagram of a player holding a ReadyState, then an input holding up
 * When a tick reads it
 * Then the player holds up: every message of a datagram is read
 */
TEST_F(GameInstanceInputTest, ReadsAnInputAfterAnotherMessage) {
  std::vector<std::byte> datagram = readyDatagram(sessionHeader(FIRST_PLAYER));
  const std::vector<std::byte> input =
      nextInputDatagram(FIRST_PLAYER, HOLDS_UP, sessionHeader(FIRST_PLAYER));
  datagram.insert(datagram.end(), input.begin() + network::PACKET_HEADER_SIZE,
                  input.end());
  sendDatagram(FIRST_PLAYER, std::move(datagram));

  deliver({FIRST_SENT});

  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_UP);
}

/**
 * Given a player whose input comes in a datagram of the next protocol version
 * When a tick reads it
 * Then no input is kept for the player
 */
TEST_F(GameInstanceInputTest, IgnoresADatagramOfAnotherVersion) {
  network::PacketHeader header = sessionHeader(FIRST_PLAYER);
  header.version = NEXT_PROTOCOL_VERSION;
  send(FIRST_PLAYER, HOLDS_UP, header);

  deliver({FIRST_SENT});

  EXPECT_FALSE(newestInputOf(FIRST_PLAYER).has_value());
}

/**
 * Given a player whose input holding up was kept during a game
 * When that game finishes and the game waits for players in a new world
 * Then the player still holds up: the client numbers its inputs for the whole
 * session, so its next input stays newer than the one kept
 */
TEST_F(GameInstanceInputTest, KeepsTheInputAcrossANewWorld) {
  send(FIRST_PLAYER, HOLDS_UP);
  for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
    sendReady({.player = player,
               .ready = true,
               .sequence = FIRST_SEQUENCE,
               .sessionToken = PLAYER_SESSION_TOKENS.at(player)});
  }
  deliver({FIRST_SENT});
  ASSERT_EQ(game().state(), GameState::InProgress);
  game().finish();

  runTicks(FINISHED_STATE_TICKS);

  ASSERT_EQ(game().state(), GameState::WaitingForPlayers);
  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_UP);
}

/**
 * Given a game in progress whose first player holds right
 * When a second of ticks runs: the tick that reads the input, then
 * SIMULATION_TICKS_PER_SECOND - 1 more
 * Then the ship of that player has drifted HELD_SECOND_DISTANCE to the right,
 * on the same row: it gathered speed from rest
 */
TEST_F(GameInstanceMovementTest, DriftsAShipTowardsTheDirectionHeld) {
  startGame();
  const game::Position start = shipPositionOf(FIRST_PLAYER);
  send(FIRST_PLAYER, HOLDS_RIGHT);

  deliver({FIRST_SENT});
  runTicks(engine::SIMULATION_TICKS_PER_SECOND - 1);

  const game::Position arrival = shipPositionOf(FIRST_PLAYER);
  EXPECT_NEAR(arrival.x, start.x + HELD_SECOND_DISTANCE, POSITION_TOLERANCE);
  EXPECT_FLOAT_EQ(arrival.y, start.y);
}

/**
 * Given a game in progress whose first player holds right and whose second
 * player holds down
 * When a second of ticks runs
 * Then each ship has drifted HELD_SECOND_DISTANCE the way its own player holds
 */
TEST_F(GameInstanceMovementTest, MovesEachShipByTheInputOfItsOwnPlayer) {
  startGame();
  const game::Position firstStart = shipPositionOf(FIRST_PLAYER);
  const game::Position secondStart = shipPositionOf(SECOND_PLAYER);
  send(FIRST_PLAYER, HOLDS_RIGHT);
  send(SECOND_PLAYER, HOLDS_DOWN);

  deliver({FIRST_SENT, SECOND_SENT});
  runTicks(engine::SIMULATION_TICKS_PER_SECOND - 1);

  const game::Position firstArrival = shipPositionOf(FIRST_PLAYER);
  const game::Position secondArrival = shipPositionOf(SECOND_PLAYER);
  EXPECT_NEAR(firstArrival.x, firstStart.x + HELD_SECOND_DISTANCE,
              POSITION_TOLERANCE);
  EXPECT_FLOAT_EQ(firstArrival.y, firstStart.y);
  EXPECT_FLOAT_EQ(secondArrival.x, secondStart.x);
  EXPECT_NEAR(secondArrival.y, secondStart.y + HELD_SECOND_DISTANCE,
              POSITION_TOLERANCE);
}

/**
 * Given a game in progress
 * When the first player sends down, nothing, down and nothing, read in one
 * tick, then the ticks of a whole roll run
 * Then its ship is ROLL_DISTANCE lower, on the same column: the server
 * read the double press and rolled the ship, no direction held to drift it
 */
TEST_F(GameInstanceMovementTest, RollsAShipOnADoublePressOfDown) {
  startGame();
  const game::Position start = shipPositionOf(FIRST_PLAYER);
  send(FIRST_PLAYER, HOLDS_DOWN);
  send(FIRST_PLAYER, HOLDS_NOTHING);
  send(FIRST_PLAYER, HOLDS_DOWN);
  send(FIRST_PLAYER, HOLDS_NOTHING);

  deliver({FIRST_SENT, SECOND_SENT, THIRD_SENT, FOURTH_SENT});
  runTicks(ROLL_END_TICK - 1);

  const game::Position arrival = shipPositionOf(FIRST_PLAYER);
  EXPECT_NEAR(arrival.y, start.y + ROLL_DISTANCE, POSITION_TOLERANCE);
  EXPECT_FLOAT_EQ(arrival.x, start.x);
}

/**
 * Given a game in progress whose first player holds right and whose second
 * player sent no input
 * When ticks run
 * Then the ship of the second player stays where it appeared
 */
TEST_F(GameInstanceMovementTest, LeavesTheShipOfAPlayerWithoutInputWhereItIs) {
  startGame();
  const game::Position start = shipPositionOf(SECOND_PLAYER);
  send(FIRST_PLAYER, HOLDS_RIGHT);

  deliver({FIRST_SENT});
  runTicks(SOME_TICKS);

  const game::Position arrival = shipPositionOf(SECOND_PLAYER);
  EXPECT_FLOAT_EQ(arrival.x, start.x);
  EXPECT_FLOAT_EQ(arrival.y, start.y);
}

/**
 * Given a game in progress whose first player holds shoot and nothing else
 * When ticks run
 * Then its ship stays where it appeared: shooting is not a direction
 */
TEST_F(GameInstanceMovementTest, DoesNotMoveAShipWhosePlayerOnlyShoots) {
  startGame();
  const game::Position start = shipPositionOf(FIRST_PLAYER);
  send(FIRST_PLAYER, HOLDS_SHOOT);

  deliver({FIRST_SENT});
  runTicks(SOME_TICKS);

  const game::Position arrival = shipPositionOf(FIRST_PLAYER);
  EXPECT_FLOAT_EQ(arrival.x, start.x);
  EXPECT_FLOAT_EQ(arrival.y, start.y);
}

/**
 * Given a game waiting for players whose first player holds right
 * When ticks run
 * Then its ship stays where it appeared: ships move only while the game is in
 * progress
 */
TEST_F(GameInstanceMovementTest, MovesNoShipWhileWaitingForPlayers) {
  const game::Position start = shipPositionOf(FIRST_PLAYER);
  send(FIRST_PLAYER, HOLDS_RIGHT);

  deliver({FIRST_SENT});
  runTicks(SOME_TICKS);

  const game::Position arrival = shipPositionOf(FIRST_PLAYER);
  EXPECT_FLOAT_EQ(arrival.x, start.x);
  EXPECT_FLOAT_EQ(arrival.y, start.y);
}

/**
 * Given a finished game whose first player holds right
 * When fewer ticks run than the game stays finished
 * Then its ship stays where the game left it
 */
TEST_F(GameInstanceMovementTest, MovesNoShipWhileFinished) {
  startGame();
  game().finish();
  const game::Position start = shipPositionOf(FIRST_PLAYER);
  send(FIRST_PLAYER, HOLDS_RIGHT);

  deliver({FIRST_SENT});
  runTicks(SOME_TICKS);

  ASSERT_EQ(game().state(), GameState::Finished);
  const game::Position arrival = shipPositionOf(FIRST_PLAYER);
  EXPECT_FLOAT_EQ(arrival.x, start.x);
  EXPECT_FLOAT_EQ(arrival.y, start.y);
}

/**
 * Given a game in progress whose first player holds right
 * When the ticks run up to the first WorldState sent after the input
 * Then that WorldState places the ship where the world has it at the end of
 * that tick: the ships move before the state is sent, not a tick later
 */
TEST_F(GameInstanceMovementTest, SendsTheShipWhereItMovedInTheSameTick) {
  startGame();
  const game::Position start = shipPositionOf(FIRST_PLAYER);
  send(FIRST_PLAYER, HOLDS_RIGHT);

  deliver({FIRST_SENT});
  runTicks(TICKS_PER_WORLD_STATE - 1);

  const game::Position moved = shipPositionOf(FIRST_PLAYER);
  ASSERT_NE(moved.x, start.x);
  const std::vector<network::OutgoingDatagram> states =
      ofType(network::MessageType::WorldState, sentTo(FIRST_PLAYER));
  ASSERT_EQ(states.size(), 2U);
  const auto state = onlyMessageIn<network::WorldState>(states.back());
  ASSERT_EQ(state.tick, TICKS_PER_WORLD_STATE);
  const std::optional<network::EntityState> ship =
      entityOf(state, shipIdOf(FIRST_PLAYER));
  ASSERT_TRUE(ship.has_value());
  const network::EntityState sent = ship.value_or(network::EntityState{});
  EXPECT_NEAR(sent.x, moved.x, WIRE_COORDINATE_TOLERANCE);
  EXPECT_NEAR(sent.y, moved.y, WIRE_COORDINATE_TOLERANCE);
}

/**
 * Given a game in progress whose first player holds one arrow
 * When the tick that reads the input runs
 * Then the ship of that player has moved along the axis of the arrow, the way
 * it points, as far as a ship at rest drifts in its first tick, and not along
 * the other axis
 */
TEST_P(GameInstanceArrowTest, MovesTheShipTheWayOfTheArrowHeld) {
  const HeldArrow arrow = GetParam();
  startGame();
  const game::Position start = shipPositionOf(FIRST_PLAYER);
  send(FIRST_PLAYER, arrow.actions);

  deliver({FIRST_SENT});

  const game::Position arrival = shipPositionOf(FIRST_PLAYER);
  EXPECT_NEAR(arrival.x, start.x + (FIRST_TICK_DISTANCE * arrow.towardsX),
              POSITION_TOLERANCE);
  EXPECT_NEAR(arrival.y, start.y + (FIRST_TICK_DISTANCE * arrow.towardsY),
              POSITION_TOLERANCE);
}

INSTANTIATE_TEST_SUITE_P(EachArrow, GameInstanceArrowTest,
                         ::testing::Values(HeldArrow{.name = "Up",
                                                     .actions = HOLDS_UP,
                                                     .towardsX = 0.0F,
                                                     .towardsY = -1.0F},
                                           HeldArrow{.name = "Down",
                                                     .actions = HOLDS_DOWN,
                                                     .towardsX = 0.0F,
                                                     .towardsY = 1.0F},
                                           HeldArrow{.name = "Left",
                                                     .actions = HOLDS_LEFT,
                                                     .towardsX = -1.0F,
                                                     .towardsY = 0.0F},
                                           HeldArrow{.name = "Right",
                                                     .actions = HOLDS_RIGHT,
                                                     .towardsX = 1.0F,
                                                     .towardsY = 0.0F}));

/**
 * Given a game with two players
 * When an input arrives from an address and port that is not one of theirs
 * Then the journal holds a line about the datagram dropped that names that
 * address and port
 */
TEST_F(GameInstanceLogTest, LogsADatagramFromAStrangerWithItsSender) {
  queueStrangersInput();

  tick();

  const std::vector<std::string> lines = dropLines();
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_NE(lines.front().find(senderText(playerEndpoint(STRANGER))),
            std::string::npos);
}

/**
 * Given a game with two players
 * When the first player's input arrives under the next protocol version
 * Then the journal holds a line about the datagram dropped that names the
 * address and port of that player
 */
TEST_F(GameInstanceLogTest, LogsADatagramOfAnotherVersionWithItsSender) {
  network::PacketHeader header = sessionHeader(FIRST_PLAYER);
  header.version = NEXT_PROTOCOL_VERSION;
  send(FIRST_PLAYER, HOLDS_UP, header);

  deliver({FIRST_SENT});

  const std::vector<std::string> lines = dropLines();
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_NE(lines.front().find(senderText(playerEndpoint(FIRST_PLAYER))),
            std::string::npos);
}

/**
 * Given a stranger's input dropped and logged on the first tick
 * When another one arrives on the last tick before REJECTION_LINE_INTERVAL
 * of ticks has run
 * Then the journal still holds one line about them
 */
TEST_F(GameInstanceLogTest, WritesNoSecondLineBeforeTheIntervalOfTicksIsOver) {
  queueStrangersInput();
  tick();
  runTicks(TICKS_PER_REJECTION_LINE - 2);

  queueStrangersInput();
  tick();

  EXPECT_EQ(dropLines().size(), 1U);
}

/**
 * Given a stranger's input dropped and logged on the first tick
 * When another one arrives on the tick at which REJECTION_LINE_INTERVAL of
 * ticks has run
 * Then the journal holds a second line: a whole interval of ticks is exactly
 * that interval of time
 */
TEST_F(GameInstanceLogTest, WritesTheNextLineOnceTheIntervalOfTicksIsOver) {
  queueStrangersInput();
  tick();
  runTicks(TICKS_PER_REJECTION_LINE - 1);

  queueStrangersInput();
  tick();

  EXPECT_EQ(dropLines().size(), 2U);
}

/**
 * Given a game with two players, who send a Heartbeat before every tick
 * When a tick reads them
 * Then the journal holds no line about a message without a handler: a
 * Heartbeat is read, and says nothing more than that its sender is there
 */
TEST_F(GameInstanceLogTest, ReadsAHeartbeatWithoutLoggingIt) {
  tick();

  EXPECT_TRUE(journal().linesHolding(SKIPPED_MESSAGE_LINE_TEXT).empty());
}

/**
 * Given a game with two players, the second of whom goes silent
 * When the timeout runs
 * Then the journal holds one line saying that player 2 timed out
 */
TEST_F(GameInstanceLogTest, LogsThePlayerWhoTimedOut) {
  timeOut(SECOND_PLAYER);

  EXPECT_EQ(
      journal().linesHolding(testing::SECOND_PLAYER_TIMED_OUT_LINE_TEXT).size(),
      1U);
}

/**
 * Given a game with two players
 * When the second sends a Disconnection
 * Then the journal holds one line saying that player 2 left, which names its
 * address and port, and none saying that it timed out
 */
TEST_F(GameInstanceLogTest, LogsThePlayerWhoLeftWithItsSender) {
  leave(SECOND_PLAYER);
  tick();
  runTicks(PROTOCOL_SESSION_TIMEOUT_TICKS + 1);

  const std::vector<std::string> lines =
      journal().linesHolding(testing::SECOND_PLAYER_LEFT_LINE_TEXT);
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_NE(lines.front().find(senderText(playerEndpoint(SECOND_PLAYER))),
            std::string::npos);
  EXPECT_TRUE(journal()
                  .linesHolding(testing::SECOND_PLAYER_TIMED_OUT_LINE_TEXT)
                  .empty());
}

/**
 * Given a game in progress whose two players go silent
 * When both time out, and the game stays finished for a while
 * Then the journal holds one line saying the game finished with no player left
 */
TEST_F(GameInstanceLogTest, LogsOnceThatTheGameFinishedWithNoPlayerLeft) {
  for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
    sendReady({.player = player, .ready = true, .sequence = FIRST_SEQUENCE});
  }
  tick();
  silence(FIRST_PLAYER);

  timeOut(SECOND_PLAYER);
  runTicks(SOME_TICKS);

  EXPECT_EQ(journal()
                .linesHolding(testing::GAME_LEFT_BY_EVERY_PLAYER_LINE_TEXT)
                .size(),
            1U);
}

/**
 * Given a room whose two players go silent
 * When both time out, and the empty room keeps waiting
 * Then no line says that a game finished
 */
TEST_F(GameInstanceLogTest, LogsNoFinishedGameForAnEmptyRoom) {
  silence(FIRST_PLAYER);

  timeOut(SECOND_PLAYER);
  runTicks(SOME_TICKS);

  EXPECT_TRUE(journal()
                  .linesHolding(testing::GAME_LEFT_BY_EVERY_PLAYER_LINE_TEXT)
                  .empty());
}

/**
 * Given a player who sent SHUFFLED_INPUT_COUNT inputs, each holding other
 * actions
 * When they arrive shuffled, some of them twice, DATAGRAMS_PER_TICK per tick
 * Then the player holds the actions of the last input sent
 */
TEST_P(GameInstanceInputShuffleTest,
       KeepsTheLastInputSentWhateverTheArrivalOrder) {
  for (std::uint32_t input = 0; input < SHUFFLED_INPUT_COUNT; ++input) {
    send(FIRST_PLAYER, static_cast<std::uint8_t>(input));
  }
  const std::vector<network::IncomingDatagram> arrivals =
      network().shuffledArrivals(GetParam());

  deliverInTicks(arrivals);

  EXPECT_EQ(heldActions(FIRST_PLAYER),
            static_cast<std::uint8_t>(SHUFFLED_INPUT_COUNT - 1));
}

INSTANTIATE_TEST_SUITE_P(Seeds, GameInstanceInputShuffleTest,
                         ::testing::Range(std::uint32_t{0},
                                          SHUFFLE_SEED_COUNT));

}  // namespace rtype::server
