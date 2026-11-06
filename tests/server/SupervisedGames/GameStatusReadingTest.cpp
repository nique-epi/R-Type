#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "DatagramCodec.hpp"
#include "GameStatus.hpp"
#include "GameStatusReading.hpp"
#include "Heartbeat.hpp"
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ReceivedStatus.hpp"
#include "SupervisedGamesTestConstants.hpp"

using rtype::network::GameStatus;
using rtype::network::GameStatusState;
using rtype::network::IncomingDatagram;
using rtype::network::PacketHeader;
using rtype::server::readGameStatus;
using rtype::server::ReceivedStatus;

/**
 * Given a datagram of a game that holds a status, a token and a sequence
 * When the status is read
 * Then it is the status, with that token and that sequence
 */
TEST(GameStatusReading, ReadsTheStatusTheTokenAndTheSequence) {
  const IncomingDatagram datagram{
      .sender = {},
      .payload = rtype::network::DatagramCodec::encode(
          PacketHeader{.sessionToken = FIRST_LINK_TOKEN,
                       .sequence = SEQUENCE_FIVE},
          GameStatus{.state = GameStatusState::InProgress, .playerCount = 3})};

  const auto received = readGameStatus(datagram);

  ASSERT_TRUE(received.has_value());
  const ReceivedStatus status = received.value_or(ReceivedStatus{});
  EXPECT_EQ(status.token, FIRST_LINK_TOKEN);
  EXPECT_EQ(status.sequence, SEQUENCE_FIVE);
  EXPECT_EQ(status.status.playerCount, 3);
}

/**
 * Given a datagram that holds a heartbeat and no status
 * When the status is read
 * Then there is none
 */
TEST(GameStatusReading, FindsNoStatusInADatagramWithoutOne) {
  const IncomingDatagram datagram{
      .sender = {},
      .payload = rtype::network::DatagramCodec::encode(
          PacketHeader{.sessionToken = FIRST_LINK_TOKEN},
          rtype::network::Heartbeat{})};

  EXPECT_FALSE(readGameStatus(datagram).has_value());
}

/**
 * Given a datagram shorter than a header
 * When the status is read
 * Then there is none and nothing is thrown
 */
TEST(GameStatusReading, FindsNoStatusInATooShortDatagram) {
  const IncomingDatagram datagram{.sender = {},
                                  .payload = std::vector<std::byte>(3)};

  EXPECT_FALSE(readGameStatus(datagram).has_value());
}

/**
 * Given a datagram whose status announces five players
 * When the status is read
 * Then there is none and nothing is thrown
 */
TEST(GameStatusReading, RefusesAStatusThatAnnouncesTooManyPlayers) {
  auto payload = rtype::network::DatagramCodec::encode(
      PacketHeader{.sessionToken = FIRST_LINK_TOKEN},
      GameStatus{.state = GameStatusState::InProgress, .playerCount = 1});
  payload.back() = std::byte{FIVE_PLAYERS_BYTE};
  const IncomingDatagram datagram{.sender = {}, .payload = payload};

  EXPECT_FALSE(readGameStatus(datagram).has_value());
}
