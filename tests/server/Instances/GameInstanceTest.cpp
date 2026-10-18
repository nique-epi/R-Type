#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "EntityIdConstants.hpp"
#include "GameInstance.hpp"
#include "GameInstanceConstants.hpp"
#include "GameInstanceTestConstants.hpp"
#include "GameState.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "ReadyState.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server {
namespace {

using testing::FIRST_PLAYER;
using testing::FIRST_SEQUENCE;
using testing::FOURTH_PLAYER;
using testing::FOURTH_SEQUENCE;
using testing::SECOND_PLAYER;
using testing::SECOND_SEQUENCE;
using testing::SOME_TICKS;
using testing::THIRD_SEQUENCE;

network::Endpoint playerEndpoint(std::size_t player) {
  return network::Endpoint{.address = std::string{testing::PLAYER_ADDRESS},
                           .port = testing::FIVE_PLAYER_PORTS.at(player)};
}

/** @brief A ReadyState sent by a player, with the header it travels under. */
struct SentReadyState {
  std::size_t player{};
  bool ready{};
  std::uint16_t sequence{};
  std::uint64_t sessionToken{testing::SESSION_TOKEN};
};

/** @brief A game whose datagrams are recorded and whose input is queued. */
class GameInstanceTest : public ::testing::Test {
 protected:
  /** @brief Connects the player number @p index + 1 and hands it to the game.
   */
  ConnectedPlayer& join(std::size_t index) {
    ConnectedPlayer* const player = game().roster().add(
        playerEndpoint(index), std::string{testing::PLAYER_NICKNAME},
        testing::SESSION_TOKEN);
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

  [[nodiscard]] GameInstance& game() { return game_; }

 private:
  std::vector<network::OutgoingDatagram> sent_;
  GameInstance game_{[this](network::OutgoingDatagram datagram) {
    sent_.push_back(std::move(datagram));
  }};
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
 * Given a connected player
 * When a ReadyState from its address carries another session token
 * Then the player stays not ready
 */
TEST_F(GameInstanceTest, IgnoresAReadyStateWithAnotherSessionToken) {
  const ConnectedPlayer& player = join(FIRST_PLAYER);
  sendReady({.player = FIRST_PLAYER,
             .ready = true,
             .sequence = FIRST_SEQUENCE,
             .sessionToken = testing::OTHER_SESSION_TOKEN});

  tick();

  EXPECT_FALSE(player.ready);
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

}  // namespace
}  // namespace rtype::server
