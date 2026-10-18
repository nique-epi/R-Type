#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "BoundedQueue.hpp"
#include "DatagramSorter.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ServerTestConstants.hpp"

using rtype::concurrency::BoundedQueue;
using rtype::network::Endpoint;
using rtype::network::IncomingDatagram;
using rtype::network::NO_SESSION_TOKEN;
using rtype::server::DatagramSorter;
using rtype::server::testing::NEXT_PROTOCOL_VERSION;
using rtype::server::testing::ONE_FLAG_SET;
using rtype::server::testing::OTHER_PROTOCOL_ID;
using rtype::server::testing::OTHER_SESSION_TOKEN;
using rtype::server::testing::SESSION_TOKEN;

namespace {

Endpoint player() {
  return Endpoint{.address = std::string{SORTED_SENDER_ADDRESS},
                  .port = PLAYER_PORT};
}

Endpoint stranger() {
  return Endpoint{.address = std::string{SORTED_SENDER_ADDRESS},
                  .port = STRANGER_PORT};
}

/** @returns A header carrying @p sessionToken, as an admitted player sends. */
rtype::network::PacketHeader playerHeader(
    std::uint64_t sessionToken = SESSION_TOKEN) {
  rtype::network::PacketHeader header;
  header.sessionToken = sessionToken;
  return header;
}

/** @returns A ReadyState carrying @p sessionToken. */
std::vector<std::byte> playerReady(std::uint64_t sessionToken = SESSION_TOKEN) {
  return readyDatagram(playerHeader(sessionToken));
}

/** @brief A sorter in front of two queues of the server's sizes. */
class DatagramSorterTest : public ::testing::Test {
 protected:
  void sort(const Endpoint& sender, const std::vector<std::byte>& datagram) {
    sorter_.sort(sender, datagram);
  }

  void admit(const Endpoint& endpoint,
             std::uint64_t sessionToken = SESSION_TOKEN) {
    sorter_.admit(endpoint, sessionToken);
  }

  [[nodiscard]] std::size_t droppedCount() const {
    return sorter_.droppedCount();
  }

  [[nodiscard]] std::size_t wrongTokenCount() const {
    return sorter_.wrongTokenCount();
  }

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

  sort(player(), playerReady());

  EXPECT_EQ(drainPlayers(),
            (std::vector<IncomingDatagram>{
                {.sender = player(), .payload = playerReady()}}));
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
  rtype::network::PacketHeader header = playerHeader();
  header.version = NEXT_PROTOCOL_VERSION;

  sort(player(), readyDatagram(header));

  EXPECT_TRUE(drainPlayers().empty());
  EXPECT_TRUE(drainRequests().empty());
}

/**
 * Given a sorter that admitted nobody
 * When a stranger sends a connection request of another protocol version
 * Then the request reaches no queue
 */
TEST_F(DatagramSorterTest, DropsAStrangersRequestOfAnotherVersion) {
  rtype::network::PacketHeader header;
  header.version = NEXT_PROTOCOL_VERSION;

  sort(stranger(), requestDatagram(header));

  EXPECT_TRUE(drainRequests().empty());
  EXPECT_TRUE(drainPlayers().empty());
}

/**
 * Given a sorter that admitted nobody
 * When a stranger sends a connection request of another protocol id
 * Then the request reaches no queue
 */
TEST_F(DatagramSorterTest, DropsAStrangersRequestOfAnotherProtocol) {
  rtype::network::PacketHeader header;
  header.protocolId = OTHER_PROTOCOL_ID;

  sort(stranger(), requestDatagram(header));

  EXPECT_TRUE(drainRequests().empty());
  EXPECT_TRUE(drainPlayers().empty());
}

/**
 * Given a sorter that admitted nobody
 * When a stranger sends a connection request with a flag set
 * Then the request reaches no queue
 */
TEST_F(DatagramSorterTest, DropsAStrangersRequestWithAFlagSet) {
  rtype::network::PacketHeader header;
  header.flags = ONE_FLAG_SET;

  sort(stranger(), requestDatagram(header));

  EXPECT_TRUE(drainRequests().empty());
  EXPECT_TRUE(drainPlayers().empty());
}

/**
 * Given an admitted player
 * When the player's datagram arrives with its last byte cut off
 * Then it reaches no queue, and nothing is thrown
 */
TEST_F(DatagramSorterTest, DropsATruncatedDatagram) {
  admit(player());
  std::vector<std::byte> datagram = playerReady();
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
 * Given an admitted player
 * When a stranger's ReadyState, then a truncated datagram, bytes shorter than
 * a header and a datagram of another version from the player arrive, then a
 * request and a datagram of the player
 * Then the sorter counts the four datagrams it dropped, and only those
 */
TEST_F(DatagramSorterTest, CountsEveryDatagramItDrops) {
  admit(player());
  std::vector<std::byte> truncated = playerReady();
  truncated.pop_back();
  rtype::network::PacketHeader otherVersion = playerHeader();
  otherVersion.version = NEXT_PROTOCOL_VERSION;
  const std::vector<std::pair<Endpoint, std::vector<std::byte>>> dropped{
      {stranger(), readyDatagram()},
      {player(), truncated},
      {player(),
       std::vector<std::byte>(CLIENT_PAYLOAD_SIZE, CLIENT_PAYLOAD_BYTE)},
      {player(), readyDatagram(otherVersion)}};

  for (const auto& [sender, datagram] : dropped) {
    sort(sender, datagram);
  }
  sort(stranger(), requestDatagram());
  sort(player(), playerReady());

  EXPECT_EQ(droppedCount(), dropped.size());
}

/**
 * Given an admitted player whose datagram waits in the player queue
 * When strangers then flood the server with twice more requests than the
 * player queue holds
 * Then the player's datagram is still the one in the player queue, and the
 * request queue holds no more than its room
 */
TEST_F(DatagramSorterTest, KeepsAPlayersDatagramThroughAFloodOfRequests) {
  admit(player());
  sort(player(), playerReady());

  for (std::size_t request = 0; request < FLOODING_REQUEST_COUNT; ++request) {
    sort(stranger(), requestDatagram());
  }

  EXPECT_EQ(drainPlayers(),
            (std::vector<IncomingDatagram>{
                {.sender = player(), .payload = playerReady()}}));
  EXPECT_EQ(drainRequests().size(),
            rtype::server::CONNECTION_REQUEST_QUEUE_CAPACITY);
}

/**
 * Given a player admitted twice
 * When the player sends one datagram
 * Then the player queue holds it once
 */
TEST_F(DatagramSorterTest, GivesAPlayerAdmittedTwiceEachDatagramOnce) {
  admit(player());
  admit(player());

  sort(player(), playerReady());

  EXPECT_EQ(drainPlayers().size(), 1);
}

/**
 * Given an admitted player
 * When a datagram from its address carries another session token
 * Then it reaches no queue, and it is counted as a wrong token only
 */
TEST_F(DatagramSorterTest, DropsAPlayersDatagramWithAnotherToken) {
  admit(player());

  sort(player(), playerReady(OTHER_SESSION_TOKEN));

  EXPECT_TRUE(drainPlayers().empty());
  EXPECT_EQ(wrongTokenCount(), 1);
  EXPECT_EQ(droppedCount(), 0);
}

/**
 * Given an admitted player
 * When a datagram from its address carries no session token
 * Then it reaches no queue, and it is counted as a wrong token
 */
TEST_F(DatagramSorterTest, DropsAPlayersDatagramWithoutToken) {
  admit(player());

  sort(player(), playerReady(NO_SESSION_TOKEN));

  EXPECT_TRUE(drainPlayers().empty());
  EXPECT_EQ(wrongTokenCount(), 1);
}

/**
 * Given an admitted player
 * When it sends a connection request carrying its session token
 * Then the request goes to the request queue
 */
TEST_F(DatagramSorterTest, SendsAPlayersRequestWithItsTokenToTheRequestQueue) {
  admit(player());

  sort(player(), requestDatagram(playerHeader()));

  EXPECT_EQ(drainRequests().size(), 1);
}

/**
 * Given an admitted player
 * When a connection request from its address carries another session token
 * Then the request reaches no queue, and it is counted as a wrong token
 */
TEST_F(DatagramSorterTest, DropsAPlayersRequestWithAnotherToken) {
  admit(player());

  sort(player(), requestDatagram(playerHeader(OTHER_SESSION_TOKEN)));

  EXPECT_TRUE(drainRequests().empty());
  EXPECT_EQ(wrongTokenCount(), 1);
}

/**
 * Given a player admitted with a token, then again with another one
 * When datagrams carrying each token arrive from it
 * Then only the one carrying the newer token reaches the player queue
 */
TEST_F(DatagramSorterTest, KeepsOnlyTheNewerTokenOfAPlayerAdmittedAgain) {
  admit(player(), SESSION_TOKEN);
  admit(player(), OTHER_SESSION_TOKEN);

  sort(player(), playerReady(SESSION_TOKEN));
  sort(player(), playerReady(OTHER_SESSION_TOKEN));

  EXPECT_EQ(
      drainPlayers(),
      (std::vector<IncomingDatagram>{
          {.sender = player(), .payload = playerReady(OTHER_SESSION_TOKEN)}}));
  EXPECT_EQ(wrongTokenCount(), 1);
}
