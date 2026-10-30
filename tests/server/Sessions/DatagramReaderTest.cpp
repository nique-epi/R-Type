#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include "DatagramReader.hpp"
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "SentDatagrams.hpp"
#include "ServerDatagrams.hpp"
#include "SessionsFixtures.hpp"
#include "SessionsTestConstants.hpp"

namespace rtype::server {

using testing::addPlayer;
using testing::FIRST_SEQUENCE;
using testing::LAST_SEQUENCE;
using testing::NEWER_SEQUENCE;
using testing::OLDER_SEQUENCE;
using testing::playerEndpoint;
using testing::SOME_SEQUENCE;

namespace {

/** @returns A Heartbeat the player at index 0 sends in the datagram of
 * sequence @p sequence. */
network::IncomingDatagram heartbeatOfSequence(std::uint16_t sequence) {
  return network::IncomingDatagram{
      .sender = playerEndpoint(0),
      .payload =
          heartbeatDatagram(network::PacketHeader{.sequence = sequence})};
}

}  // namespace

/**
 * Given a reader and a player who sent nothing yet
 * When a datagram of sequence 40 arrives from the player
 * Then it is readable
 */
TEST(DatagramReader, ReadsTheFirstDatagramOfAPlayer) {
  PlayerRoster roster;
  static_cast<void>(addPlayer(roster, 0));
  DatagramReader reader(roster);

  EXPECT_TRUE(reader.read(heartbeatOfSequence(SOME_SEQUENCE)).has_value());
}

/**
 * Given a reader that read the datagram of sequence 40 of a player
 * When a copy of that datagram arrives
 * Then it is not readable
 */
TEST(DatagramReader, DropsADuplicateOfTheNewestDatagram) {
  PlayerRoster roster;
  static_cast<void>(addPlayer(roster, 0));
  DatagramReader reader(roster);
  ASSERT_TRUE(reader.read(heartbeatOfSequence(SOME_SEQUENCE)).has_value());

  EXPECT_FALSE(reader.read(heartbeatOfSequence(SOME_SEQUENCE)).has_value());
}

/**
 * Given a reader that read the datagram of sequence 40 of a player
 * When the datagram of sequence 39 arrives, late
 * Then it is not readable, and the datagram of sequence 41 still is
 */
TEST(DatagramReader, DropsADatagramOlderThanTheNewestOne) {
  PlayerRoster roster;
  static_cast<void>(addPlayer(roster, 0));
  DatagramReader reader(roster);
  ASSERT_TRUE(reader.read(heartbeatOfSequence(SOME_SEQUENCE)).has_value());

  EXPECT_FALSE(reader.read(heartbeatOfSequence(OLDER_SEQUENCE)).has_value());
  EXPECT_TRUE(reader.read(heartbeatOfSequence(NEWER_SEQUENCE)).has_value());
}

/**
 * Given a reader that read the datagram of sequence 65535 of a player
 * When the datagram of sequence 0 arrives
 * Then it is readable, since sequence 0 follows sequence 65535
 */
TEST(DatagramReader, ReadsADatagramOnceTheSequenceWrapsAround) {
  PlayerRoster roster;
  static_cast<void>(addPlayer(roster, 0));
  DatagramReader reader(roster);
  ASSERT_TRUE(reader.read(heartbeatOfSequence(LAST_SEQUENCE)).has_value());

  EXPECT_TRUE(reader.read(heartbeatOfSequence(FIRST_SEQUENCE)).has_value());
}

/**
 * Given two players, the first having sent the datagram of sequence 40
 * When the second sends a datagram of sequence 5
 * Then it is readable: each player has an order of its own
 */
TEST(DatagramReader, KeepsAnOrderForEachPlayer) {
  PlayerRoster roster;
  static_cast<void>(addPlayer(roster, 0));
  static_cast<void>(addPlayer(roster, 1));
  DatagramReader reader(roster);
  ASSERT_TRUE(reader.read(heartbeatOfSequence(SOME_SEQUENCE)).has_value());
  const network::IncomingDatagram fromSecond{
      .sender = playerEndpoint(1),
      .payload = heartbeatDatagram(network::PacketHeader{.sequence = 5})};

  EXPECT_TRUE(reader.read(fromSecond).has_value());
}

}  // namespace rtype::server
