#include <gtest/gtest.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "ByteWriter.hpp"
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "EntityIdConstants.hpp"
#include "GameInstance.hpp"
#include "GameInstanceConstants.hpp"
#include "GameInstanceTestConstants.hpp"
#include "GameState.hpp"
#include "IncomingDatagram.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "ReadyState.hpp"
#include "ServerDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "UnreliableNetwork.hpp"

namespace rtype::server {
namespace {

using testing::DATAGRAMS_PER_TICK;
using testing::FIRST_PLAYER;
using testing::FIRST_SENT;
using testing::FIRST_SEQUENCE;
using testing::FOURTH_PLAYER;
using testing::FOURTH_SENT;
using testing::FOURTH_SEQUENCE;
using testing::HOLDS_DOWN;
using testing::HOLDS_LEFT;
using testing::HOLDS_SHOOT;
using testing::HOLDS_UP;
using testing::LARGEST_INPUT_NUMBER;
using testing::NEXT_PROTOCOL_VERSION;
using testing::PLAYER_SESSION_TOKENS;
using testing::SECOND_PLAYER;
using testing::SECOND_SENT;
using testing::SECOND_SEQUENCE;
using testing::SHUFFLE_SEED_COUNT;
using testing::SHUFFLED_INPUT_COUNT;
using testing::SOME_TICKS;
using testing::STRANGER;
using testing::THIRD_SENT;
using testing::THIRD_SEQUENCE;
using testing::UNLISTED_ACTION;

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
 * Given a player who sent up, then a datagram from the address of that player
 * carrying the session token of the other player and an input numbered
 * LARGEST_INPUT_NUMBER holding down, then shoot
 * When one tick receives the three
 * Then the player holds shoot: the forged input does not lock the ship
 */
TEST_F(GameInstanceInputTest, IgnoresAnInputCarryingAnotherSessionToken) {
  send(FIRST_PLAYER, HOLDS_UP);
  sendDatagram(
      FIRST_PLAYER,
      inputDatagram(network::PlayerInput{.inputNumber = LARGEST_INPUT_NUMBER,
                                         .actions = HOLDS_DOWN},
                    sessionHeader(SECOND_PLAYER)));
  send(FIRST_PLAYER, HOLDS_SHOOT);

  deliver({FIRST_SENT, SECOND_SENT, THIRD_SENT});

  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_SHOOT);
}

/**
 * Given a datagram from the address of a player carrying the session token of
 * another player
 * When a tick reads it
 * Then it is counted as a datagram with another token
 */
TEST_F(GameInstanceInputTest, CountsADatagramCarryingAnotherSessionToken) {
  send(FIRST_PLAYER, HOLDS_UP, sessionHeader(SECOND_PLAYER));

  deliver({FIRST_SENT});

  EXPECT_EQ(game().foreignTokenCount(), 1U);
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
