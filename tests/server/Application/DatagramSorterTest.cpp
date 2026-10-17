#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include <vector>
#include "BoundedQueue.hpp"
#include "DatagramSorter.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ServerConstants.hpp"
#include "ServerDatagrams.hpp"
#include "ServerTestConstants.hpp"

using rtype::concurrency::BoundedQueue;
using rtype::network::Endpoint;
using rtype::network::IncomingDatagram;
using rtype::server::DatagramSorter;

namespace {

Endpoint player() {
  return Endpoint{.address = std::string{SORTED_SENDER_ADDRESS},
                  .port = PLAYER_PORT};
}

Endpoint stranger() {
  return Endpoint{.address = std::string{SORTED_SENDER_ADDRESS},
                  .port = STRANGER_PORT};
}

/** @brief A sorter in front of two queues of the server's sizes. */
class DatagramSorterTest : public ::testing::Test {
 protected:
  void sort(const Endpoint& sender, const std::vector<std::byte>& datagram) {
    sorter_.sort(sender, datagram);
  }

  void admit(const Endpoint& endpoint) { sorter_.admit(endpoint); }

  [[nodiscard]] std::vector<IncomingDatagram> drainPlayers() {
    std::vector<IncomingDatagram> drained;
    players_.drainInto(drained);
    return drained;
  }

  [[nodiscard]] std::vector<IncomingDatagram> drainRequests() {
    std::vector<IncomingDatagram> drained;
    connectionRequests_.drainInto(drained);
    return drained;
  }

 private:
  BoundedQueue<IncomingDatagram> players_{
      rtype::server::INCOMING_DATAGRAM_QUEUE_CAPACITY};
  BoundedQueue<IncomingDatagram> connectionRequests_{
      rtype::server::CONNECTION_REQUEST_QUEUE_CAPACITY};
  DatagramSorter sorter_{DatagramSorter::Destinations{
      .players = &players_, .connectionRequests = &connectionRequests_}};
};

}  // namespace

/**
 * Given a sorter that admitted nobody
 * When a stranger sends a connection request
 * Then the request goes to the request queue, with its sender, and nothing to
 * the player queue
 */
TEST_F(DatagramSorterTest, SendsAStrangersRequestToTheRequestQueue) {
  sort(stranger(), requestDatagram());

  EXPECT_EQ(drainRequests(),
            (std::vector<IncomingDatagram>{
                {.sender = stranger(), .payload = requestDatagram()}}));
  EXPECT_TRUE(drainPlayers().empty());
}

/**
 * Given a sorter that admitted nobody
 * When a stranger sends anything but a connection request
 * Then the datagram reaches no queue
 */
TEST_F(DatagramSorterTest, DropsAStrangersOtherDatagram) {
  sort(stranger(), readyDatagram());

  EXPECT_TRUE(drainPlayers().empty());
  EXPECT_TRUE(drainRequests().empty());
}

/**
 * Given an admitted player
 * When the player sends a datagram that is not a connection request
 * Then it goes to the player queue
 */
TEST_F(DatagramSorterTest, SendsAPlayersDatagramToThePlayerQueue) {
  admit(player());

  sort(player(), readyDatagram());

  EXPECT_EQ(drainPlayers(),
            (std::vector<IncomingDatagram>{
                {.sender = player(), .payload = readyDatagram()}}));
}

/**
 * Given an admitted player whose acceptance was lost
 * When the player sends its connection request again
 * Then the request goes to the request queue, where it is answered
 */
TEST_F(DatagramSorterTest, SendsAPlayersRequestToTheRequestQueue) {
  admit(player());

  sort(player(), requestDatagram());

  EXPECT_EQ(drainRequests().size(), 1);
  EXPECT_TRUE(drainPlayers().empty());
}

/**
 * Given an admitted player
 * When the player sends a datagram of another protocol version
 * Then the datagram reaches no queue
 */
TEST_F(DatagramSorterTest, DropsADatagramOfAnotherVersion) {
  admit(player());
  rtype::network::PacketHeader header;
  header.version = NEXT_PROTOCOL_VERSION;

  sort(player(), readyDatagram(header));

  EXPECT_TRUE(drainPlayers().empty());
}

/**
 * Given an admitted player
 * When the player's datagram arrives with its last byte cut off
 * Then it reaches no queue, and nothing is thrown
 */
TEST_F(DatagramSorterTest, DropsATruncatedDatagram) {
  admit(player());
  std::vector<std::byte> datagram = readyDatagram();
  datagram.pop_back();

  EXPECT_NO_THROW(sort(player(), datagram));
  EXPECT_TRUE(drainPlayers().empty());
}

/**
 * Given an admitted player
 * When bytes shorter than a header arrive from the player
 * Then they reach no queue, and nothing is thrown
 */
TEST_F(DatagramSorterTest, DropsBytesShorterThanAHeader) {
  admit(player());
  const std::vector<std::byte> bytes(CLIENT_PAYLOAD_SIZE, CLIENT_PAYLOAD_BYTE);

  EXPECT_NO_THROW(sort(player(), bytes));
  EXPECT_TRUE(drainPlayers().empty());
}

/**
 * Given an admitted player whose datagram waits in the player queue
 * When strangers then flood the server with ten times more requests than the
 * request queue holds
 * Then the player's datagram is still the one in the player queue, and the
 * request queue holds no more than its room
 */
TEST_F(DatagramSorterTest, KeepsAPlayersDatagramThroughAFloodOfRequests) {
  admit(player());
  sort(player(), readyDatagram());

  for (std::size_t request = 0; request < FLOODING_REQUEST_COUNT; ++request) {
    sort(stranger(), requestDatagram());
  }

  EXPECT_EQ(drainPlayers(),
            (std::vector<IncomingDatagram>{
                {.sender = player(), .payload = readyDatagram()}}));
  EXPECT_EQ(drainRequests().size(),
            rtype::server::CONNECTION_REQUEST_QUEUE_CAPACITY);
}

/**
 * Given a player admitted twice
 * When the player sends one datagram
 * Then the player queue holds it once
 */
TEST_F(DatagramSorterTest, AdmitsAPlayerOnceWhenAdmittedAgain) {
  admit(player());
  admit(player());

  sort(player(), readyDatagram());

  EXPECT_EQ(drainPlayers().size(), 1);
}
