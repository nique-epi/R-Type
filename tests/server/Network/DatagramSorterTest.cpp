#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "BoundedQueue.hpp"
#include "DatagramSorter.hpp"
#include "DebugJournal.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "NetworkConstants.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ServerTestConstants.hpp"
#include "SimulatedClock.hpp"
#include "TimeConstants.hpp"

using rtype::concurrency::BoundedQueue;
using rtype::network::Endpoint;
using rtype::network::IncomingDatagram;
using rtype::network::NO_SESSION_TOKEN;
using rtype::server::DatagramSorter;
using rtype::server::testing::DROPPED_DATAGRAM_LINE_TEXT;
using rtype::server::testing::DROPPED_TOGETHER_COUNT;
using rtype::server::testing::NEXT_PROTOCOL_VERSION;
using rtype::server::testing::ONE_FLAG_SET;
using rtype::server::testing::OTHER_PROTOCOL_ID;
using rtype::server::testing::OTHER_SESSION_TOKEN;
using rtype::server::testing::REFUSAL_DATAGRAM_SIZE;
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

Endpoint secondPlayer() {
  return Endpoint{.address = std::string{SORTED_SENDER_ADDRESS},
                  .port = SECOND_PLAYER_PORT};
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

  void forget(const Endpoint& endpoint,
              std::uint64_t sessionToken = SESSION_TOKEN) {
    sorter_.forget(endpoint, sessionToken);
  }

  void advanceClock(rtype::engine::Duration elapsed) {
    clock_.advance(elapsed);
  }

  [[nodiscard]] std::size_t droppedCount() const {
    return sorter_.droppedCount();
  }

  [[nodiscard]] std::size_t wrongTokenCount() const {
    return sorter_.wrongTokenCount();
  }

  [[nodiscard]] std::optional<Endpoint> takeWrongTokenSender() {
    return sorter_.takeWrongTokenSender();
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
  SimulatedClock clock_;
  DatagramSorter sorter_{
      DatagramSorter::Destinations{.players = &players_,
                                   .connectionRequests = &connectionRequests_},
      clock_};
};

/** @brief The sorter, with the log written to a journal the test reads. */
class DatagramSorterLogTest : public DatagramSorterTest {
 protected:
  /** @returns The lines of the journal about a datagram dropped. */
  [[nodiscard]] std::vector<std::string> dropLines() const {
    return journal_.linesHolding(DROPPED_DATAGRAM_LINE_TEXT);
  }

 private:
  DebugJournal journal_;
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
 * Then the request goes to the request queue, where it is refused, and
 * nothing to the player queue
 */
TEST_F(DatagramSorterTest, SendsAStrangersRequestOfAnotherVersionToBeRefused) {
  rtype::network::PacketHeader header;
  header.version = NEXT_PROTOCOL_VERSION;

  sort(stranger(), requestDatagram(header));

  EXPECT_EQ(drainRequests(),
            (std::vector<IncomingDatagram>{
                {.sender = stranger(), .payload = requestDatagram(header)}}));
  EXPECT_TRUE(drainPlayers().empty());
}

/**
 * Given a sorter that admitted nobody
 * When a stranger sends a datagram of another version of 26 bytes, the size of
 * the refusal, whatever it holds
 * Then it goes to the request queue
 */
TEST_F(DatagramSorterTest, SendsADatagramOfAnotherVersionAsLongAsTheRefusal) {
  rtype::network::PacketHeader header;
  header.version = NEXT_PROTOCOL_VERSION;
  const std::vector<std::byte> datagram = readyDatagram(header);
  ASSERT_EQ(datagram.size(), REFUSAL_DATAGRAM_SIZE);

  sort(stranger(), datagram);

  EXPECT_EQ(drainRequests().size(), 1U);
}

/**
 * Given a sorter that admitted nobody
 * When a stranger sends a datagram of another version of 25 bytes, one fewer
 * than the refusal
 * Then it reaches no queue: a refusal would send more bytes than it received
 */
TEST_F(DatagramSorterTest,
       DropsADatagramOfAnotherVersionShorterThanTheRefusal) {
  rtype::network::PacketHeader header;
  header.version = NEXT_PROTOCOL_VERSION;
  const std::vector<std::byte> datagram = heartbeatDatagram(header);
  ASSERT_EQ(datagram.size(), REFUSAL_DATAGRAM_SIZE - 1);

  sort(stranger(), datagram);

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

/**
 * Given a player admitted, then forgotten with its token
 * When it sends a datagram that is not a connection request
 * Then the datagram reaches no queue, and it is counted like a stranger's
 */
TEST_F(DatagramSorterTest, DropsAForgottenPlayersDatagramLikeAStrangers) {
  admit(player());
  forget(player());

  sort(player(), playerReady());

  EXPECT_TRUE(drainPlayers().empty());
  EXPECT_EQ(droppedCount(), 1);
  EXPECT_EQ(wrongTokenCount(), 0);
}

/**
 * Given a player admitted, then forgotten with its token
 * When a connection request without token comes from its address
 * Then the request goes to the request queue, as a stranger's does
 */
TEST_F(DatagramSorterTest, SendsAForgottenPlayersRequestToTheRequestQueue) {
  admit(player());
  forget(player());

  sort(player(), requestDatagram());

  EXPECT_EQ(drainRequests().size(), 1);
}

/**
 * Given a player admitted with a token, then again with another one
 * When the session of the first token is forgotten
 * Then the datagrams carrying the newer token still reach the player queue
 */
TEST_F(DatagramSorterTest, KeepsTheNewerSessionWhenTheOlderIsForgotten) {
  admit(player(), SESSION_TOKEN);
  admit(player(), OTHER_SESSION_TOKEN);
  forget(player(), SESSION_TOKEN);

  sort(player(), playerReady(OTHER_SESSION_TOKEN));

  EXPECT_EQ(drainPlayers().size(), 1);
}

/**
 * Given two admitted players
 * When the first is forgotten
 * Then the datagrams of the second still reach the player queue
 */
TEST_F(DatagramSorterTest, ForgetsOnlyThePlayerItIsGiven) {
  admit(player());
  admit(secondPlayer());
  forget(player());

  sort(secondPlayer(), playerReady());

  EXPECT_EQ(drainPlayers().size(), 1);
}

/**
 * Given a player admitted, forgotten, then admitted again with a new token
 * When it sends a datagram carrying the new token
 * Then the datagram reaches the player queue
 */
TEST_F(DatagramSorterTest, AdmitsAForgottenPlayerAgain) {
  admit(player(), SESSION_TOKEN);
  forget(player(), SESSION_TOKEN);
  admit(player(), OTHER_SESSION_TOKEN);

  sort(player(), playerReady(OTHER_SESSION_TOKEN));

  EXPECT_EQ(drainPlayers().size(), 1);
}

/**
 * Given two admitted players
 * When a datagram with another session token comes from the address of each,
 * the first player's first
 * Then the sorter names the first player as the sender
 */
TEST_F(DatagramSorterTest, NamesTheFirstSenderWithAnotherToken) {
  admit(player());
  admit(secondPlayer());

  sort(player(), playerReady(OTHER_SESSION_TOKEN));
  sort(secondPlayer(), playerReady(OTHER_SESSION_TOKEN));

  EXPECT_EQ(takeWrongTokenSender(), player());
}

/**
 * Given two admitted players, and the sender of a datagram with another token
 * from the first already taken
 * When a datagram with another token comes from the second
 * Then the sorter names the second: each take starts afresh
 */
TEST_F(DatagramSorterTest, NamesTheFirstSenderSinceThePreviousTake) {
  admit(player());
  admit(secondPlayer());
  sort(player(), playerReady(OTHER_SESSION_TOKEN));
  ASSERT_EQ(takeWrongTokenSender(), player());

  sort(secondPlayer(), playerReady(OTHER_SESSION_TOKEN));

  EXPECT_EQ(takeWrongTokenSender(), secondPlayer());
}

/**
 * Given an admitted player
 * When a stranger sends a datagram, which is dropped
 * Then the sorter names no sender: only a wrong session token is named
 */
TEST_F(DatagramSorterTest, NamesNoSenderForAnotherDrop) {
  admit(player());

  sort(stranger(), playerReady(OTHER_SESSION_TOKEN));

  EXPECT_EQ(takeWrongTokenSender(), std::nullopt);
}

/**
 * Given a sorter that admitted nobody
 * When a stranger sends a ReadyState
 * Then the journal holds a line about the datagram dropped that names the
 * address and the port of the stranger
 */
TEST_F(DatagramSorterLogTest, LogsADroppedDatagramWithItsSender) {
  sort(stranger(), playerReady());

  const std::vector<std::string> lines = dropLines();
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_NE(lines.front().find(senderText(stranger())), std::string::npos);
}

/**
 * Given an admitted player
 * When a datagram arrives from the address of the player with another session
 * token
 * Then the journal holds a line about the datagram dropped that names that
 * address and port
 */
TEST_F(DatagramSorterLogTest, LogsADatagramWithAnotherTokenWithItsSender) {
  admit(player());

  sort(player(), playerReady(OTHER_SESSION_TOKEN));

  const std::vector<std::string> lines = dropLines();
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_NE(lines.front().find(senderText(player())), std::string::npos);
}

/**
 * Given an admitted player
 * When the player sends a ReadyState and a stranger a connection request
 * Then the journal holds no line about a datagram dropped: both went to a
 * queue
 */
TEST_F(DatagramSorterLogTest, LogsNothingForTheDatagramsItQueues) {
  admit(player());

  sort(player(), playerReady());
  sort(stranger(), requestDatagram());

  EXPECT_TRUE(dropLines().empty());
}

/**
 * Given a sorter that admitted nobody
 * When a stranger sends DROPPED_TOGETHER_COUNT ReadyState at the same time
 * Then the journal holds one line about them, not one per datagram: whoever
 * floods the server cannot flood the log
 */
TEST_F(DatagramSorterLogTest, WritesOneLineForDatagramsDroppedTogether) {
  for (std::size_t sent = 0; sent < DROPPED_TOGETHER_COUNT; ++sent) {
    sort(stranger(), playerReady());
  }

  EXPECT_EQ(dropLines().size(), 1U);
}

/**
 * Given a sorter that logged a datagram it dropped
 * When the clock it was given moves on by REJECTION_LINE_INTERVAL and a
 * stranger's datagram is dropped again
 * Then the journal holds a second line: the interval is measured on that
 * clock
 */
TEST_F(DatagramSorterLogTest, WritesTheNextLineOnceTheIntervalIsOver) {
  sort(stranger(), playerReady());

  advanceClock(rtype::network::REJECTION_LINE_INTERVAL);
  sort(stranger(), playerReady());

  EXPECT_EQ(dropLines().size(), 2U);
}
