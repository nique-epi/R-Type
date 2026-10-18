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
#include "IncomingDatagram.hpp"
#include "MessageType.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "PlayerInputReceiver.hpp"
#include "PlayerInputReceiverTestConstants.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "ServerDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "UnreliableNetwork.hpp"

namespace rtype::server {
namespace {

using testing::DATAGRAMS_PER_TICK;
using testing::FIRST_PLAYER;
using testing::FIRST_SENT;
using testing::FIVE_PLAYER_PORTS;
using testing::FOURTH_SENT;
using testing::HOLDS_DOWN;
using testing::HOLDS_LEFT;
using testing::HOLDS_SHOOT;
using testing::HOLDS_UP;
using testing::LARGEST_INPUT_NUMBER;
using testing::NEXT_PROTOCOL_VERSION;
using testing::PLAYER_ADDRESS;
using testing::PLAYER_NICKNAME;
using testing::PLAYER_SESSION_TOKENS;
using testing::SECOND_PLAYER;
using testing::SECOND_SENT;
using testing::SHUFFLE_SEED_COUNT;
using testing::SHUFFLED_INPUT_COUNT;
using testing::STRANGER;
using testing::THIRD_SENT;
using testing::UNLISTED_ACTION;

network::Endpoint playerEndpoint(std::size_t player) {
  return network::Endpoint{.address = std::string{PLAYER_ADDRESS},
                           .port = FIVE_PLAYER_PORTS.at(player)};
}

/** @returns A header carrying the session token of @p player. */
network::PacketHeader sessionHeader(std::size_t player) {
  network::PacketHeader header;
  header.sessionToken = PLAYER_SESSION_TOKENS.at(player);
  return header;
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

/**
 * @brief Two players in a roster, a receiver reading what reaches the server,
 * and the network in between. Each player numbers its inputs from 1 in the
 * order it sends them, as a client does.
 */
class PlayerInputReceiverTest : public ::testing::Test {
 protected:
  PlayerInputReceiverTest() {
    for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
      EXPECT_NE(
          roster_.add(playerEndpoint(player), std::string{PLAYER_NICKNAME},
                      PLAYER_SESSION_TOKENS.at(player)),
          nullptr);
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

  /** @brief One tick receives the datagrams sent at @p sendIndexes. */
  void receive(std::initializer_list<std::size_t> sendIndexes) {
    const std::vector<network::IncomingDatagram> arrivals =
        network_.arrivals(sendIndexes);
    receiver_.receive(arrivals);
  }

  /**
   * @brief Ticks receive @p arrivals in order, DATAGRAMS_PER_TICK at a time.
   */
  void receiveInTicks(std::span<const network::IncomingDatagram> arrivals) {
    while (!arrivals.empty()) {
      const std::size_t count = std::min(DATAGRAMS_PER_TICK, arrivals.size());
      receiver_.receive(arrivals.first(count));
      arrivals = arrivals.subspan(count);
    }
  }

  /** @returns The number of the last input @p player sent. */
  [[nodiscard]] std::uint32_t lastInputNumber(std::size_t player) const {
    return inputsSent_.at(player);
  }

  /** @returns The input the server keeps for @p player, if any. */
  [[nodiscard]] std::optional<network::PlayerInput> newestInputOf(
      std::size_t player) {
    const ConnectedPlayer* const connected =
        roster_.find(playerEndpoint(player));
    if (connected == nullptr) {
      ADD_FAILURE() << "player " << player << " is not in the roster";
      return std::nullopt;
    }
    return connected->newestInput;
  }

  /** @returns The actions of the input the server keeps for @p player. */
  [[nodiscard]] std::optional<std::uint8_t> heldActions(std::size_t player) {
    const std::optional<network::PlayerInput> newest = newestInputOf(player);
    if (!newest.has_value()) {
      return std::nullopt;
    }
    return newest->actions;
  }

  [[nodiscard]] const UnreliableNetwork& network() const { return network_; }

  [[nodiscard]] const PlayerInputReceiver& receiver() const {
    return receiver_;
  }

 private:
  PlayerRoster roster_;
  PlayerInputReceiver receiver_{roster_};
  UnreliableNetwork network_;
  std::array<std::uint32_t, PLAYER_SESSION_TOKENS.size()> inputsSent_{};
};

/**
 * Given a player whose datagram holds a ReadyState and no input
 * When the server receives it
 * Then no input is kept for the player
 */
TEST_F(PlayerInputReceiverTest, KeepsNoInputFromADatagramWithoutOne) {
  sendDatagram(FIRST_PLAYER, readyDatagram(sessionHeader(FIRST_PLAYER)));

  receive({FIRST_SENT});

  EXPECT_FALSE(newestInputOf(FIRST_PLAYER).has_value());
}

/**
 * Given a player who sent one input holding up
 * When the server receives it
 * Then the player holds up
 */
TEST_F(PlayerInputReceiverTest, KeepsTheFirstInputOfAPlayer) {
  send(FIRST_PLAYER, HOLDS_UP);

  receive({FIRST_SENT});

  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_UP);
}

/**
 * Given a player who sent up, then down, and whose up arrived in a tick
 * When down arrives in the next tick
 * Then the player holds down
 */
TEST_F(PlayerInputReceiverTest, KeepsANewerInputOfALaterTick) {
  send(FIRST_PLAYER, HOLDS_UP);
  send(FIRST_PLAYER, HOLDS_DOWN);
  receive({FIRST_SENT});

  receive({SECOND_SENT});

  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_DOWN);
}

/**
 * Given a player who sent up, then down, and whose down arrived first
 * When up arrives in the next tick, late
 * Then the player still holds down
 */
TEST_F(PlayerInputReceiverTest, IgnoresAnOlderInputArrivingInALaterTick) {
  send(FIRST_PLAYER, HOLDS_UP);
  send(FIRST_PLAYER, HOLDS_DOWN);
  receive({SECOND_SENT});

  receive({FIRST_SENT});

  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_DOWN);
}

/**
 * Given a player who sent up, down, then shoot
 * When one tick receives them as shoot, up, down
 * Then the player holds shoot, the last one sent, not down, the last one
 * received
 */
TEST_F(PlayerInputReceiverTest, KeepsTheHighestInputOfATickWhateverTheOrder) {
  send(FIRST_PLAYER, HOLDS_UP);
  send(FIRST_PLAYER, HOLDS_DOWN);
  send(FIRST_PLAYER, HOLDS_SHOOT);

  receive({THIRD_SENT, FIRST_SENT, SECOND_SENT});

  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_SHOOT);
}

/**
 * Given a player who sent up, then down, both received
 * When a duplicate of up arrives in a later tick
 * Then the player still holds down
 */
TEST_F(PlayerInputReceiverTest, IgnoresALateDuplicate) {
  send(FIRST_PLAYER, HOLDS_UP);
  send(FIRST_PLAYER, HOLDS_DOWN);
  receive({FIRST_SENT, SECOND_SENT});

  receive({FIRST_SENT});

  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_DOWN);
}

/**
 * Given a player whose input holding up was kept
 * When another input with the same number arrives, holding down
 * Then the player still holds up: an input is replaced only by a higher number
 */
TEST_F(PlayerInputReceiverTest, IgnoresAnInputNumberedLikeTheOneKept) {
  send(FIRST_PLAYER, HOLDS_UP);
  sendDatagram(
      FIRST_PLAYER,
      inputDatagram(
          network::PlayerInput{.inputNumber = lastInputNumber(FIRST_PLAYER),
                               .actions = HOLDS_DOWN},
          sessionHeader(FIRST_PLAYER)));

  receive({FIRST_SENT, SECOND_SENT});

  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_UP);
}

/**
 * Given a first player who sent three inputs, and a second player who sent
 * one holding left
 * When one tick receives the four
 * Then the second player holds left: the numbers of one player never count
 * for another
 */
TEST_F(PlayerInputReceiverTest, KeepsTheInputsOfEachPlayerApart) {
  send(FIRST_PLAYER, HOLDS_UP);
  send(FIRST_PLAYER, HOLDS_DOWN);
  send(FIRST_PLAYER, HOLDS_SHOOT);
  send(SECOND_PLAYER, HOLDS_LEFT);

  receive({FIRST_SENT, SECOND_SENT, THIRD_SENT, FOURTH_SENT});

  EXPECT_EQ(heldActions(SECOND_PLAYER), HOLDS_LEFT);
}

/**
 * Given a player who sent up, then a datagram from the address of that player
 * carrying the session token of the other player and an input numbered
 * LARGEST_INPUT_NUMBER holding down, then shoot
 * When one tick receives the three
 * Then the player holds shoot: the forged input does not lock the ship
 */
TEST_F(PlayerInputReceiverTest, IgnoresAnInputCarryingAnotherSessionToken) {
  send(FIRST_PLAYER, HOLDS_UP);
  sendDatagram(
      FIRST_PLAYER,
      inputDatagram(network::PlayerInput{.inputNumber = LARGEST_INPUT_NUMBER,
                                         .actions = HOLDS_DOWN},
                    sessionHeader(SECOND_PLAYER)));
  send(FIRST_PLAYER, HOLDS_SHOOT);

  receive({FIRST_SENT, SECOND_SENT, THIRD_SENT});

  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_SHOOT);
}

/**
 * Given a datagram from the address of a player carrying the session token of
 * another player
 * When the server receives it
 * Then it is counted as a datagram with another token
 */
TEST_F(PlayerInputReceiverTest, CountsADatagramCarryingAnotherSessionToken) {
  send(FIRST_PLAYER, HOLDS_UP, sessionHeader(SECOND_PLAYER));

  receive({FIRST_SENT});

  EXPECT_EQ(receiver().foreignTokenCount(), 1U);
}

/**
 * Given a sender that is not in the roster, using the address of the players
 * and the session token of the first player
 * When the server receives its input
 * Then the first player has no input: inputs are kept for the sender, never
 * for the owner of the token
 */
TEST_F(PlayerInputReceiverTest, IgnoresAnInputFromASenderNotInTheRoster) {
  sendDatagram(
      STRANGER,
      inputDatagram(network::PlayerInput{.inputNumber = LARGEST_INPUT_NUMBER,
                                         .actions = HOLDS_DOWN},
                    sessionHeader(FIRST_PLAYER)));

  receive({FIRST_SENT});

  EXPECT_FALSE(newestInputOf(FIRST_PLAYER).has_value());
}

/**
 * Given a player who sent an input numbered LARGEST_INPUT_NUMBER whose
 * actions hold a bit the protocol does not list, then an input holding up
 * When one tick receives both
 * Then the player holds up: the refused input was not kept
 */
TEST_F(PlayerInputReceiverTest, IgnoresAnInputItsReaderRefuses) {
  sendDatagram(
      FIRST_PLAYER,
      network::DatagramCodec::encode(
          sessionHeader(FIRST_PLAYER), network::MessageType::PlayerInput,
          inputPayload(network::PlayerInput{.inputNumber = LARGEST_INPUT_NUMBER,
                                            .actions = UNLISTED_ACTION})));
  send(FIRST_PLAYER, HOLDS_UP);

  receive({FIRST_SENT, SECOND_SENT});

  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_UP);
}

/**
 * Given a player whose first datagram lost its last byte, then who sent up
 * When one tick receives both
 * Then nothing is thrown and the player holds up
 */
TEST_F(PlayerInputReceiverTest, ReadsTheDatagramAfterATruncatedOne) {
  std::vector<std::byte> truncated =
      nextInputDatagram(FIRST_PLAYER, HOLDS_DOWN, sessionHeader(FIRST_PLAYER));
  truncated.pop_back();
  sendDatagram(FIRST_PLAYER, std::move(truncated));
  send(FIRST_PLAYER, HOLDS_UP);

  EXPECT_NO_THROW(receive({FIRST_SENT, SECOND_SENT}));
  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_UP);
}

/**
 * Given a datagram of a player holding a ReadyState, then an input holding up
 * When the server receives it
 * Then the player holds up: every message of a datagram is read
 */
TEST_F(PlayerInputReceiverTest, ReadsAnInputAfterAnotherMessage) {
  std::vector<std::byte> datagram = readyDatagram(sessionHeader(FIRST_PLAYER));
  const std::vector<std::byte> input =
      nextInputDatagram(FIRST_PLAYER, HOLDS_UP, sessionHeader(FIRST_PLAYER));
  datagram.insert(datagram.end(), input.begin() + network::PACKET_HEADER_SIZE,
                  input.end());
  sendDatagram(FIRST_PLAYER, std::move(datagram));

  receive({FIRST_SENT});

  EXPECT_EQ(heldActions(FIRST_PLAYER), HOLDS_UP);
}

/**
 * Given a player whose input comes in a datagram of the next protocol version
 * When the server receives it
 * Then no input is kept for the player
 */
TEST_F(PlayerInputReceiverTest, IgnoresADatagramOfAnotherVersion) {
  network::PacketHeader header = sessionHeader(FIRST_PLAYER);
  header.version = NEXT_PROTOCOL_VERSION;
  send(FIRST_PLAYER, HOLDS_UP, header);

  receive({FIRST_SENT});

  EXPECT_FALSE(newestInputOf(FIRST_PLAYER).has_value());
}

/** @brief The receiver fixture, once for each seed of a shuffled delivery. */
class PlayerInputReceiverShuffleTest
    : public PlayerInputReceiverTest,
      public ::testing::WithParamInterface<std::uint32_t> {};

/**
 * Given a player who sent SHUFFLED_INPUT_COUNT inputs, each holding other
 * actions
 * When they arrive shuffled, some of them twice, DATAGRAMS_PER_TICK per tick
 * Then the player holds the actions of the last input sent
 */
TEST_P(PlayerInputReceiverShuffleTest,
       KeepsTheLastInputSentWhateverTheArrivalOrder) {
  for (std::uint32_t input = 0; input < SHUFFLED_INPUT_COUNT; ++input) {
    send(FIRST_PLAYER, static_cast<std::uint8_t>(input));
  }
  const std::vector<network::IncomingDatagram> arrivals =
      network().shuffledArrivals(GetParam());

  receiveInTicks(arrivals);

  EXPECT_EQ(heldActions(FIRST_PLAYER),
            static_cast<std::uint8_t>(SHUFFLED_INPUT_COUNT - 1));
}

INSTANTIATE_TEST_SUITE_P(Seeds, PlayerInputReceiverShuffleTest,
                         ::testing::Range(std::uint32_t{0},
                                          SHUFFLE_SEED_COUNT));

}  // namespace
}  // namespace rtype::server
