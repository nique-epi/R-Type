#include <gtest/gtest.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ByteWriter.hpp"
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "DebugJournal.hpp"
#include "EnemyKilled.hpp"
#include "EntityDestruction.hpp"
#include "EntityIdConstants.hpp"
#include "EventBus.hpp"
#include "GameInstance.hpp"
#include "GameInstanceConstants.hpp"
#include "GameInstanceTestConstants.hpp"
#include "GameState.hpp"
#include "IncomingDatagram.hpp"
#include "LobbyState.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "ReadyState.hpp"
#include "SentDatagrams.hpp"
#include "ServerConstants.hpp"
#include "ServerDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShipControlConstants.hpp"
#include "Spawning/BydoSpawnerTestConstants.hpp"
#include "TimeConstants.hpp"
#include "UnreliableNetwork.hpp"
#include "WorldState.hpp"
#include "game/ShipControl/ShipControlTestConstants.hpp"

namespace rtype::server {
namespace {

constexpr std::size_t INTERVALS_OF_BYDOS = 10;

using game::testing::POSITION_TOLERANCE;
using testing::DATAGRAMS_PER_TICK;
using testing::DROPPED_DATAGRAM_LINE_TEXT;
using testing::FIRST_PLAYER;
using testing::FIRST_SENT;
using testing::FIRST_SEQUENCE;
using testing::FOURTH_PLAYER;
using testing::FOURTH_SENT;
using testing::FOURTH_SEQUENCE;
using testing::HOLDS_DOWN;
using testing::HOLDS_LEFT;
using testing::HOLDS_RIGHT;
using testing::HOLDS_SHOOT;
using testing::HOLDS_UP;
using testing::LARGEST_INPUT_NUMBER;
using testing::NEXT_PROTOCOL_VERSION;
using testing::onlyMessageIn;
using testing::onlyMessageTypeOf;
using testing::PLAYER_SESSION_TOKENS;
using testing::playerEndpoint;
using testing::SECOND_PLAYER;
using testing::SECOND_SENT;
using testing::SECOND_SEQUENCE;
using testing::SHUFFLE_SEED_COUNT;
using testing::SHUFFLED_INPUT_COUNT;
using testing::SOME_TICKS;
using testing::STRANGER;
using testing::THIRD_SENT;
using testing::THIRD_SEQUENCE;
using testing::TICKS_PER_REJECTION_LINE;
using testing::UNLISTED_ACTION;
using testing::WIRE_COORDINATE_TOLERANCE;

constexpr std::uint32_t SOME_SEED = 7;
constexpr std::uint32_t SOME_POINTS = 100;

/** @brief A ReadyState sent by a player, with the header it travels under. */
struct SentReadyState {
  std::size_t player{};
  bool ready{};
  std::uint16_t sequence{};
  std::uint64_t sessionToken{testing::SESSION_TOKEN};
};

/** @returns The type of the only message of each of @p datagrams, in order. */
std::vector<network::MessageType> messageTypesOf(
    const std::vector<network::OutgoingDatagram>& datagrams) {
  std::vector<network::MessageType> types;
  types.reserve(datagrams.size());
  for (const network::OutgoingDatagram& datagram : datagrams) {
    types.push_back(onlyMessageTypeOf(datagram));
  }
  return types;
}

/** @returns Those of @p datagrams whose only message is of type @p type, in
 * order. */
std::vector<network::OutgoingDatagram> ofType(
    network::MessageType type,
    const std::vector<network::OutgoingDatagram>& datagrams) {
  std::vector<network::OutgoingDatagram> kept;
  for (const network::OutgoingDatagram& datagram : datagrams) {
    if (onlyMessageTypeOf(datagram) == type) {
      kept.push_back(datagram);
    }
  }
  return kept;
}

/** @returns The entity of @p state whose id is @p entityId, or nothing when the
 * state does not list it. */
std::optional<network::EntityState> entityOf(const network::WorldState& state,
                                             std::uint32_t entityId) {
  for (const network::EntityState& entity : state.entities) {
    if (entity.entityId == entityId) {
      return entity;
    }
  }
  return std::nullopt;
}

/** @returns The room of a game whose only player, the first, is not ready. */
network::LobbyState roomOfTheFirstPlayerNotReady() {
  return network::LobbyState{
      .players = {network::LobbyPlayer{
          .playerNumber = network::FIRST_PLAYER_NUMBER,
          .ready = false,
          .nickname = std::string{testing::PLAYER_NICKNAME}}}};
}

/** @brief A game whose datagrams are recorded and whose input is queued. */
class GameInstanceTest : public ::testing::Test {
 protected:
  /**
   * @brief Connects the player number @p index + 1, under @p sessionToken, and
   * hands it to the game.
   */
  ConnectedPlayer& join(std::size_t index,
                        std::uint64_t sessionToken = testing::SESSION_TOKEN) {
    ConnectedPlayer* const player = game().roster().add(
        playerEndpoint(index), std::string{testing::PLAYER_NICKNAME},
        sessionToken);
    game().onPlayerJoined(*player);
    return *player;
  }

  /** @brief Queues the ReadyState described by @p sent for the next tick. */
  void sendReady(const SentReadyState& sent) {
    network::PacketHeader header;
    header.sessionToken = sent.sessionToken;
    header.sequence = sent.sequence;
    queue(network::IncomingDatagram{
        .sender = playerEndpoint(sent.player),
        .payload = network::DatagramCodec::encode(
            header, network::ReadyState{.ready = sent.ready})});
  }

  /** @brief Queues @p datagram for the next tick. */
  void queue(network::IncomingDatagram datagram) {
    incoming_.push_back(std::move(datagram));
  }

  /** @brief Runs one tick with what was queued, then empties the queue. */
  void tick() {
    game().onTick(incoming_);
    incoming_.clear();
  }

  void runTicks(std::size_t count) {
    for (std::size_t index = 0; index < count; ++index) {
      tick();
    }
  }

  /** @brief Connects one player, who says ready, and runs the tick that starts
   * the game. */
  void startGameWithOnePlayer() {
    join(FIRST_PLAYER);
    sendReady(
        {.player = FIRST_PLAYER, .ready = true, .sequence = FIRST_SEQUENCE});
    tick();
    ASSERT_EQ(game().state(), GameState::InProgress);
  }

  /** @returns The datagrams sent to the player at @p index, oldest first. */
  [[nodiscard]] std::vector<network::OutgoingDatagram> sentTo(
      std::size_t index) const {
    return testing::sentTo(sent_, playerEndpoint(index));
  }

  [[nodiscard]] GameInstance& game() { return game_; }
  [[nodiscard]] engine::EventBus& events() { return events_; }

 private:
  std::vector<network::OutgoingDatagram> sent_;
  engine::EventBus events_;
  GameInstance game_{[this](network::OutgoingDatagram datagram) {
                       sent_.push_back(std::move(datagram));
                     },
                     SOME_SEED, events_};
  std::vector<network::IncomingDatagram> incoming_;
};

/**
 * Given a game that was just built
 * When nothing happened yet
 * Then it waits for players, at tick 0
 */
TEST_F(GameInstanceTest, StartsWaitingForPlayers) {
  EXPECT_EQ(game().state(), GameState::WaitingForPlayers);
  EXPECT_EQ(game().tick(), 0);
}

/**
 * Given a game with no connected player
 * When ticks run
 * Then it keeps waiting for players
 */
TEST_F(GameInstanceTest, DoesNotStartWithoutPlayers) {
  runTicks(SOME_TICKS);

  EXPECT_EQ(game().state(), GameState::WaitingForPlayers);
}

/**
 * Given one connected player who did not say ready
 * When ticks run
 * Then the game keeps waiting
 */
TEST_F(GameInstanceTest, WaitsForAPlayerWhoIsNotReady) {
  join(FIRST_PLAYER);

  runTicks(SOME_TICKS);

  EXPECT_EQ(game().state(), GameState::WaitingForPlayers);
}

/**
 * Given one connected player
 * When the player says ready and a tick runs
 * Then the game is in progress
 */
TEST_F(GameInstanceTest, StartsWhenTheOnlyPlayerIsReady) {
  join(FIRST_PLAYER);
  sendReady(
      {.player = FIRST_PLAYER, .ready = true, .sequence = FIRST_SEQUENCE});

  tick();

  EXPECT_EQ(game().state(), GameState::InProgress);
}

/**
 * Given two connected players
 * When only the first says ready
 * Then the game keeps waiting
 */
TEST_F(GameInstanceTest, WaitsWhileOnePlayerIsNotReady) {
  join(FIRST_PLAYER);
  join(SECOND_PLAYER);
  sendReady(
      {.player = FIRST_PLAYER, .ready = true, .sequence = FIRST_SEQUENCE});

  tick();

  EXPECT_EQ(game().state(), GameState::WaitingForPlayers);
}

/**
 * Given four connected players, the most a game holds
 * When every one of them says ready
 * Then the game is in progress
 */
TEST_F(GameInstanceTest, StartsWhenFourPlayersAreReady) {
  for (std::size_t index = 0; index < network::MAXIMUM_PLAYERS; ++index) {
    join(index);
    sendReady({.player = index, .ready = true, .sequence = FIRST_SEQUENCE});
  }

  tick();

  EXPECT_EQ(game().state(), GameState::InProgress);
}

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
 * Given a game that waits for players
 * When ticks run
 * Then the tick stays at 0
 */
TEST_F(GameInstanceTest, DoesNotCountTicksWhileWaiting) {
  join(FIRST_PLAYER);

  runTicks(SOME_TICKS);

  EXPECT_EQ(game().tick(), 0);
}

/**
 * Given a game that just started
 * When SOME_TICKS ticks run
 * Then the tick is SOME_TICKS
 */
TEST_F(GameInstanceTest, CountsTicksWhileInProgress) {
  startGameWithOnePlayer();

  runTicks(SOME_TICKS);

  EXPECT_EQ(game().tick(), SOME_TICKS);
}

/**
 * Given a game in progress
 * When it is finished
 * Then its state is Finished
 */
TEST_F(GameInstanceTest, FinishEndsTheGameInProgress) {
  startGameWithOnePlayer();

  game().finish();

  EXPECT_EQ(game().state(), GameState::Finished);
}

/**
 * Given a game that waits for players
 * When it is finished
 * Then it keeps waiting
 */
TEST_F(GameInstanceTest, FinishDoesNothingWhileWaiting) {
  game().finish();

  EXPECT_EQ(game().state(), GameState::WaitingForPlayers);
}

/**
 * Given a finished game
 * When one tick less than FINISHED_STATE_TICKS runs
 * Then it is still finished
 */
TEST_F(GameInstanceTest, StaysFinishedBeforeTheFinishedDurationElapsed) {
  startGameWithOnePlayer();
  game().finish();

  runTicks(FINISHED_STATE_TICKS - 1);

  EXPECT_EQ(game().state(), GameState::Finished);
}

/**
 * Given a finished game
 * When FINISHED_STATE_TICKS ticks run
 * Then it waits for players again
 */
TEST_F(GameInstanceTest, WaitsForPlayersAgainOnceTheFinishedDurationElapsed) {
  startGameWithOnePlayer();
  game().finish();

  runTicks(FINISHED_STATE_TICKS);

  EXPECT_EQ(game().state(), GameState::WaitingForPlayers);
}

/**
 * Given a game that was played by two players and finished
 * When it waits for players again
 * Then the world holds one fresh ship per player, not the old ones as well,
 * and the tick is back at 0
 */
TEST_F(GameInstanceTest, ResetsTheWorldBetweenTwoGames) {
  join(FIRST_PLAYER);
  join(SECOND_PLAYER);
  sendReady(
      {.player = FIRST_PLAYER, .ready = true, .sequence = FIRST_SEQUENCE});
  sendReady(
      {.player = SECOND_PLAYER, .ready = true, .sequence = FIRST_SEQUENCE});
  tick();
  runTicks(SOME_TICKS);
  game().finish();

  runTicks(FINISHED_STATE_TICKS);

  EXPECT_EQ(game().world().entityCount(), 2);
  EXPECT_EQ(game().tick(), 0);
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
 * Given a game that was played and finished
 * When it waits for players again
 * Then every player is connected, not ready, and has a ship
 */
TEST_F(GameInstanceTest, KeepsThePlayersConnectedButNotReadyForTheNextGame) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);
  sendReady(
      {.player = FIRST_PLAYER, .ready = true, .sequence = FIRST_SEQUENCE});
  tick();
  game().finish();

  runTicks(FINISHED_STATE_TICKS);

  EXPECT_EQ(game().roster().connected().size(), 1);
  EXPECT_FALSE(player.ready);
  EXPECT_NE(player.shipEntityId, identifiers::NO_ENTITY_ID);
}

/**
 * Given a game that waits for players again after a game
 * When ticks run without any player saying ready
 * Then the game keeps waiting
 */
TEST_F(GameInstanceTest, NeedsEveryPlayerReadyAgainForTheNextGame) {
  startGameWithOnePlayer();
  game().finish();
  runTicks(FINISHED_STATE_TICKS);

  runTicks(SOME_TICKS);

  EXPECT_EQ(game().state(), GameState::WaitingForPlayers);
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
 * Given a game that waits for a player who is not ready
 * When as many ticks as an interval of Bydos pass
 * Then the world holds the ship only: no Bydo appears before the game starts
 */
TEST_F(GameInstanceTest, NoBydoAppearsWhileWaitingForPlayers) {
  join(FIRST_PLAYER);

  runTicks(testing::TICKS_PER_BYDO_INTERVAL);

  EXPECT_EQ(game().world().entityCount(), 1U);
}

/**
 * Given a game just started with one player
 * When one tick less than an interval passes, then one more
 * Then no Bydo is in the world, and then one is, next to the ship
 */
TEST_F(GameInstanceTest, ABydoAppearsOnceTheGameRunsForAnInterval) {
  startGameWithOnePlayer();

  runTicks(testing::TICKS_PER_BYDO_INTERVAL - 1);
  EXPECT_EQ(game().world().entityCount(), 1U);
  tick();

  EXPECT_EQ(game().world().entityCount(), 2U);
}

/**
 * Given a finished game, which had run half an interval
 * When it waits for players again, starts again, and runs one tick less than
 * an interval
 * Then no Bydo is in the new world: the timer started again from zero
 */
TEST_F(GameInstanceTest, ANewGameStartsTheBydoTimerOver) {
  startGameWithOnePlayer();
  runTicks(testing::TICKS_PER_BYDO_INTERVAL / 2);
  game().finish();
  runTicks(FINISHED_STATE_TICKS);
  ASSERT_EQ(game().state(), GameState::WaitingForPlayers);
  sendReady(
      {.player = FIRST_PLAYER, .ready = true, .sequence = SECOND_SEQUENCE});
  tick();
  ASSERT_EQ(game().state(), GameState::InProgress);

  runTicks(testing::TICKS_PER_BYDO_INTERVAL - 1);

  EXPECT_EQ(game().world().entityCount(), 1U);
}

/**
 * Given a finished game that has run for several intervals
 * When ticks run while it is finished
 * Then no Bydo appears
 */
TEST_F(GameInstanceTest, NoBydoAppearsWhileFinished) {
  startGameWithOnePlayer();
  game().finish();
  const std::size_t entitiesBefore = game().world().entityCount();

  runTicks(2 * testing::TICKS_PER_BYDO_INTERVAL);

  EXPECT_EQ(game().world().entityCount(), entitiesBefore);
}

/**
 * Given a game in progress for ten intervals of Bydos (about 20 seconds), a
 * Bydo taking about 8.5 seconds to cross the playfield
 * When the events are dispatched
 * Then the first five Bydos are gone, five removals were announced, and the
 * world holds the ship and the last five Bydos
 */
TEST_F(GameInstanceTest, ABydoThatLeavesOnTheLeftDoesNotStayInTheWorld) {
  std::size_t removals = 0;
  events().subscribe<game::EntityDestruction>(
      [&removals](const game::EntityDestruction& event) {
        removals += event.cause == game::DestructionCause::Removed;
      });
  startGameWithOnePlayer();

  runTicks(INTERVALS_OF_BYDOS * testing::TICKS_PER_BYDO_INTERVAL);
  events().dispatch();

  EXPECT_EQ(removals, 5U);
  EXPECT_EQ(game().world().entityCount(), 6U);
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
 * @returns The payload of @p input written field by field, so it may hold
 * actions encodePayload() refuses.
 */
std::vector<std::byte> inputPayload(const network::PlayerInput& input) {
  network::ByteWriter payload;
  payload.writeUint32(input.inputNumber);
  payload.writeUint8(input.actions);
  return payload.bytes();
}

/** @returns A header carrying the session token of @p player. */
network::PacketHeader sessionHeader(std::size_t player) {
  network::PacketHeader header;
  header.sessionToken = PLAYER_SESSION_TOKENS.at(player);
  return header;
}

/**
 * @brief A game with two players, each with its own session token, and the
 * network in between. Each player numbers its inputs from 1 in the order it
 * sends them, as a client does.
 */
class GameInstanceInputTest : public GameInstanceTest {
 protected:
  GameInstanceInputTest() {
    for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
      join(player, PLAYER_SESSION_TOKENS.at(player));
    }
  }

  /**
   * @returns The datagram of the next input of @p player, holding @p actions,
   * under @p header.
   */
  std::vector<std::byte> nextInputDatagram(
      std::size_t player, std::uint8_t actions,
      const network::PacketHeader& header) {
    ++inputsSent_.at(player);
    return inputDatagram(
        network::PlayerInput{.inputNumber = inputsSent_.at(player),
                             .actions = actions},
        header);
  }

  /** @brief @p player sends its next input, holding @p actions. */
  void send(std::size_t player, std::uint8_t actions) {
    send(player, actions, sessionHeader(player));
  }

  /**
   * @brief @p player sends its next input, holding @p actions, under
   * @p header.
   */
  void send(std::size_t player, std::uint8_t actions,
            const network::PacketHeader& header) {
    sendDatagram(player, nextInputDatagram(player, actions, header));
  }

  /** @brief @p datagram leaves the address and port of @p player. */
  void sendDatagram(std::size_t player, std::vector<std::byte> datagram) {
    network_.send(network::IncomingDatagram{.sender = playerEndpoint(player),
                                            .payload = std::move(datagram)});
  }

  /** @brief One tick reads the datagrams sent at @p sendIndexes. */
  void deliver(std::initializer_list<std::size_t> sendIndexes) {
    for (network::IncomingDatagram& arrival : network_.arrivals(sendIndexes)) {
      queue(std::move(arrival));
    }
    tick();
  }

  /**
   * @brief Ticks read @p arrivals in order, DATAGRAMS_PER_TICK at a time.
   */
  void deliverInTicks(std::span<const network::IncomingDatagram> arrivals) {
    while (!arrivals.empty()) {
      const std::size_t count = std::min(DATAGRAMS_PER_TICK, arrivals.size());
      for (const network::IncomingDatagram& arrival : arrivals.first(count)) {
        queue(arrival);
      }
      tick();
      arrivals = arrivals.subspan(count);
    }
  }

  /** @returns The number of the last input @p player sent. */
  [[nodiscard]] std::uint32_t lastInputNumber(std::size_t player) const {
    return inputsSent_.at(player);
  }

  /** @returns The input the game keeps for @p player, if any. */
  [[nodiscard]] std::optional<network::PlayerInput> newestInputOf(
      std::size_t player) {
    const ConnectedPlayer* const connected =
        game().roster().find(playerEndpoint(player));
    if (connected == nullptr) {
      ADD_FAILURE() << "player " << player << " is not in the roster";
      return std::nullopt;
    }
    return connected->newestInput;
  }

  /** @returns The actions of the input the game keeps for @p player. */
  [[nodiscard]] std::optional<std::uint8_t> heldActions(std::size_t player) {
    const std::optional<network::PlayerInput> newest = newestInputOf(player);
    if (!newest.has_value()) {
      return std::nullopt;
    }
    return newest->actions;
  }

  [[nodiscard]] const UnreliableNetwork& network() const { return network_; }

 private:
  UnreliableNetwork network_;
  std::array<std::uint32_t, PLAYER_SESSION_TOKENS.size()> inputsSent_{};
};

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
 * @brief The input fixture, with both players ready and a way to read where
 * their ships are.
 */
class GameInstanceMovementTest : public GameInstanceInputTest {
 protected:
  /** @brief Both players say ready, then the tick that starts the game runs. */
  void startGame() {
    for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
      sendReady({.player = player,
                 .ready = true,
                 .sequence = FIRST_SEQUENCE,
                 .sessionToken = PLAYER_SESSION_TOKENS.at(player)});
    }
    tick();
    ASSERT_EQ(game().state(), GameState::InProgress);
  }

  /** @returns The network id of the ship of @p player; NO_ENTITY_ID, after a
   * failure, when the player is not in the roster. */
  [[nodiscard]] std::uint32_t shipIdOf(std::size_t player) {
    const ConnectedPlayer* const connected =
        game().roster().find(playerEndpoint(player));
    if (connected == nullptr) {
      ADD_FAILURE() << "player " << player << " is not in the roster";
      return identifiers::NO_ENTITY_ID;
    }
    return connected->shipEntityId;
  }

  /** @returns Where the ship of @p player is; the origin, after a failure, when
   * it has none. */
  [[nodiscard]] game::Position shipPositionOf(std::size_t player) {
    const game::Position* const ship =
        game().world().shipPositionOf(shipIdOf(player));
    if (ship == nullptr) {
      ADD_FAILURE() << "player " << player << " has no ship";
      return game::Position{.x = 0.0F, .y = 0.0F};
    }
    return *ship;
  }
};

/**
 * Given a game in progress whose first player holds right
 * When a second of ticks runs: the tick that reads the input, then
 * SIMULATION_TICKS_PER_SECOND - 1 more
 * Then the ship of that player has moved SHIP_SPEED to the right, on the same
 * row
 */
TEST_F(GameInstanceMovementTest, MovesAShipAtTheShipSpeedWhileInProgress) {
  startGame();
  const game::Position start = shipPositionOf(FIRST_PLAYER);
  send(FIRST_PLAYER, HOLDS_RIGHT);

  deliver({FIRST_SENT});
  runTicks(engine::SIMULATION_TICKS_PER_SECOND - 1);

  const game::Position arrival = shipPositionOf(FIRST_PLAYER);
  EXPECT_NEAR(arrival.x, start.x + game::SHIP_SPEED, POSITION_TOLERANCE);
  EXPECT_FLOAT_EQ(arrival.y, start.y);
}

/**
 * Given a game in progress whose first player holds right and whose second
 * player holds down
 * When a second of ticks runs
 * Then each ship has moved SHIP_SPEED the way its own player holds
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
  EXPECT_NEAR(firstArrival.x, firstStart.x + game::SHIP_SPEED,
              POSITION_TOLERANCE);
  EXPECT_FLOAT_EQ(firstArrival.y, firstStart.y);
  EXPECT_FLOAT_EQ(secondArrival.x, secondStart.x);
  EXPECT_NEAR(secondArrival.y, secondStart.y + game::SHIP_SPEED,
              POSITION_TOLERANCE);
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

/** @brief An arrow a player holds alone, named after it, as its bit of the
 * actions byte, and which way it moves a ship along each axis: -1, 0 or 1. */
struct HeldArrow {
  std::string_view name;
  std::uint8_t actions{};
  float towardsX{};
  float towardsY{};
};

/** @brief Prints an arrow by its name, so the name of each test says which
 * arrow it holds. */
void PrintTo(const HeldArrow& arrow, std::ostream* output) {
  *output << arrow.name;
}

/** @brief The movement fixture, run once for each arrow. */
class GameInstanceArrowTest : public GameInstanceMovementTest,
                              public ::testing::WithParamInterface<HeldArrow> {
};

/**
 * Given a game in progress whose first player holds one arrow
 * When the tick that reads the input runs
 * Then the ship of that player has moved along the axis of the arrow, the way
 * it points, as far as SHIP_SPEED goes in one tick, and not along the other
 * axis
 */
TEST_P(GameInstanceArrowTest, MovesTheShipTheWayOfTheArrowHeld) {
  const HeldArrow arrow = GetParam();
  startGame();
  const game::Position start = shipPositionOf(FIRST_PLAYER);
  send(FIRST_PLAYER, arrow.actions);

  deliver({FIRST_SENT});

  const float oneTick =
      game::SHIP_SPEED *
      std::chrono::duration<float>(engine::SIMULATION_TICK_DURATION).count();
  const game::Position arrival = shipPositionOf(FIRST_PLAYER);
  EXPECT_NEAR(arrival.x, start.x + (oneTick * arrow.towardsX),
              POSITION_TOLERANCE);
  EXPECT_NEAR(arrival.y, start.y + (oneTick * arrow.towardsY),
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

/** @brief The input fixture, with the log written to a journal the test reads.
 */
class GameInstanceLogTest : public GameInstanceInputTest {
 protected:
  /** @brief An input from an address that is not a player's arrives. */
  void queueStrangersInput() {
    queue(network::IncomingDatagram{
        .sender = playerEndpoint(STRANGER),
        .payload = inputDatagram(
            network::PlayerInput{.inputNumber = network::FIRST_INPUT_NUMBER,
                                 .actions = HOLDS_UP},
            sessionHeader(FIRST_PLAYER))});
  }

  /** @returns The lines of the journal about a datagram dropped. */
  [[nodiscard]] std::vector<std::string> dropLines() const {
    return journal_.linesHolding(DROPPED_DATAGRAM_LINE_TEXT);
  }

 private:
  DebugJournal journal_;
};

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

/** @brief The input fixture, once for each seed of a shuffled delivery. */
class GameInstanceInputShuffleTest
    : public GameInstanceInputTest,
      public ::testing::WithParamInterface<std::uint32_t> {};

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

}  // namespace
}  // namespace rtype::server
