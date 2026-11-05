#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "DatagramSorterFixture.hpp"
#include "DatagramSorterFixtures.hpp"
#include "DatagramSorterLogFixture.hpp"
#include "DatagramSorterTestConstants.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "NetworkConstants.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "RateLimitConstantsTestConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ServerTestConstants.hpp"

using rtype::network::Endpoint;
using rtype::network::IncomingDatagram;
using rtype::network::NO_SESSION_TOKEN;
using rtype::network::testing::PROTOCOL_BAN_DURATION;
using rtype::server::testing::DROPPED_TOGETHER_COUNT;
using rtype::server::testing::NEXT_PROTOCOL_VERSION;
using rtype::server::testing::OTHER_PROTOCOL_ID;
using rtype::server::testing::OTHER_SESSION_TOKEN;
using rtype::server::testing::REFUSAL_DATAGRAM_SIZE;
using rtype::server::testing::SESSION_TOKEN;
using rtype::server::testing::UNKNOWN_FLAG;

/**
 * Given a sorter that admitted nobody
 * When a stranger sends a connection request
 * Then the request goes to the request queue, with its sender, and nothing to
 * the player queue
 */
TEST_F(DatagramSorterFixture, SendsAStrangersRequestToTheRequestQueue) {
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
TEST_F(DatagramSorterFixture, DropsAStrangersOtherDatagram) {
  sort(stranger(), readyDatagram());

  EXPECT_TRUE(drainPlayers().empty());
  EXPECT_TRUE(drainRequests().empty());
}

/**
 * Given an admitted player
 * When the player sends a datagram that is not a connection request
 * Then it goes to the player queue
 */
TEST_F(DatagramSorterFixture, SendsAPlayersDatagramToThePlayerQueue) {
  admit(player());

  sort(player(), playerReady());

  EXPECT_EQ(drainPlayers(),
            (std::vector<IncomingDatagram>{
                {.sender = player(), .payload = playerReady()}}));
}

/**
 * Given an admitted player, and the clock of the sorter at
 * SORTED_ARRIVAL_TIME
 * When the player sends a datagram
 * Then the datagram queued for the players carries that time, the time it
 * arrived
 */
TEST_F(DatagramSorterFixture, StampsEachKeptDatagramWithItsClock) {
  admit(player());
  advanceClock(SORTED_ARRIVAL_TIME);

  sort(player(), playerReady());

  const std::vector<IncomingDatagram> players = drainPlayers();
  ASSERT_EQ(players.size(), 1U);
  EXPECT_EQ(players.front().receivedAt, SORTED_ARRIVAL_TIME);
}

/**
 * Given an admitted player whose acceptance was lost
 * When the player sends its connection request again
 * Then the request goes to the request queue, where it is answered
 */
TEST_F(DatagramSorterFixture, SendsAPlayersRequestToTheRequestQueue) {
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
TEST_F(DatagramSorterFixture, DropsADatagramOfAnotherVersion) {
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
TEST_F(DatagramSorterFixture,
       SendsAStrangersRequestOfAnotherVersionToBeRefused) {
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
TEST_F(DatagramSorterFixture,
       SendsADatagramOfAnotherVersionAsLongAsTheRefusal) {
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
TEST_F(DatagramSorterFixture,
       DropsADatagramOfAnotherVersionShorterThanTheRefusal) {
  rtype::network::PacketHeader header;
  header.version = NEXT_PROTOCOL_VERSION;
  const std::vector<std::byte> datagram = disconnectionDatagram(header);
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
TEST_F(DatagramSorterFixture, DropsAStrangersRequestOfAnotherProtocol) {
  rtype::network::PacketHeader header;
  header.protocolId = OTHER_PROTOCOL_ID;

  sort(stranger(), requestDatagram(header));

  EXPECT_TRUE(drainRequests().empty());
  EXPECT_TRUE(drainPlayers().empty());
}

/**
 * Given a sorter that admitted nobody
 * When a stranger sends a connection request with a flag version 2 does not
 * know
 * Then the request reaches no queue
 */
TEST_F(DatagramSorterFixture, DropsAStrangersRequestWithAnUnknownFlag) {
  rtype::network::PacketHeader header;
  header.flags = UNKNOWN_FLAG;

  sort(stranger(), requestDatagram(header));

  EXPECT_TRUE(drainRequests().empty());
  EXPECT_TRUE(drainPlayers().empty());
}

/**
 * Given an admitted player
 * When the player's datagram arrives with its last byte cut off
 * Then it reaches no queue, and nothing is thrown
 */
TEST_F(DatagramSorterFixture, DropsATruncatedDatagram) {
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
TEST_F(DatagramSorterFixture, DropsBytesShorterThanAHeader) {
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
TEST_F(DatagramSorterFixture, CountsEveryDatagramItDrops) {
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
 * When strangers then flood the server, one request each, with twice more
 * requests than the player queue holds
 * Then the player's datagram is still the one in the player queue, and the
 * request queue holds no more than its room
 */
TEST_F(DatagramSorterFixture, KeepsAPlayersDatagramThroughAFloodOfRequests) {
  admit(player());
  sort(player(), playerReady());

  for (std::size_t request = 0; request < FLOODING_REQUEST_COUNT; ++request) {
    sort(floodingStranger(request), requestDatagram());
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
TEST_F(DatagramSorterFixture, GivesAPlayerAdmittedTwiceEachDatagramOnce) {
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
TEST_F(DatagramSorterFixture, DropsAPlayersDatagramWithAnotherToken) {
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
TEST_F(DatagramSorterFixture, DropsAPlayersDatagramWithoutToken) {
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
TEST_F(DatagramSorterFixture,
       SendsAPlayersRequestWithItsTokenToTheRequestQueue) {
  admit(player());

  sort(player(), requestDatagram(playerHeader()));

  EXPECT_EQ(drainRequests().size(), 1);
}

/**
 * Given an admitted player
 * When a connection request from its address carries another session token
 * Then the request reaches no queue, and it is counted as a wrong token
 */
TEST_F(DatagramSorterFixture, DropsAPlayersRequestWithAnotherToken) {
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
TEST_F(DatagramSorterFixture, KeepsOnlyTheNewerTokenOfAPlayerAdmittedAgain) {
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
TEST_F(DatagramSorterFixture, DropsAForgottenPlayersDatagramLikeAStrangers) {
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
TEST_F(DatagramSorterFixture, SendsAForgottenPlayersRequestToTheRequestQueue) {
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
TEST_F(DatagramSorterFixture, KeepsTheNewerSessionWhenTheOlderIsForgotten) {
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
TEST_F(DatagramSorterFixture, ForgetsOnlyThePlayerItIsGiven) {
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
TEST_F(DatagramSorterFixture, AdmitsAForgottenPlayerAgain) {
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
TEST_F(DatagramSorterFixture, NamesTheFirstSenderWithAnotherToken) {
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
TEST_F(DatagramSorterFixture, NamesTheFirstSenderSinceThePreviousTake) {
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
TEST_F(DatagramSorterFixture, NamesNoSenderForAnotherDrop) {
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
TEST_F(DatagramSorterLogFixture, LogsADroppedDatagramWithItsSender) {
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
TEST_F(DatagramSorterLogFixture, LogsADatagramWithAnotherTokenWithItsSender) {
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
TEST_F(DatagramSorterLogFixture, LogsNothingForTheDatagramsItQueues) {
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
TEST_F(DatagramSorterLogFixture, WritesOneLineForDatagramsDroppedTogether) {
  sortRepeatedly(stranger(), playerReady(), DROPPED_TOGETHER_COUNT);

  EXPECT_EQ(dropLines().size(), 1U);
}

/**
 * Given a sorter that logged a datagram it dropped
 * When the clock it was given moves on by REJECTION_LINE_INTERVAL and a
 * stranger's datagram is dropped again
 * Then the journal holds a second line: the interval is measured on that
 * clock
 */
TEST_F(DatagramSorterLogFixture, WritesTheNextLineOnceTheIntervalIsOver) {
  sort(stranger(), playerReady());

  advanceClock(rtype::network::REJECTION_LINE_INTERVAL);
  sort(stranger(), playerReady());

  EXPECT_EQ(dropLines().size(), 2U);
}

/**
 * Given a sorter that admitted nobody
 * When a stranger sends twelve connection requests at once
 * Then eleven reach the request queue: the twelfth is over its limit
 */
TEST_F(DatagramSorterFixture, PlacesNoMoreThanElevenRequestsOfAStrangerAtOnce) {
  sortRepeatedly(stranger(), requestDatagram(), REQUESTS_PLACED_AT_ONCE + 1);

  EXPECT_EQ(drainRequests().size(), REQUESTS_PLACED_AT_ONCE);
}

/**
 * Given a stranger whose connection request waits in the request queue
 * When another stranger then floods the server with twice more requests than
 * the player queue holds
 * Then the request of the first stranger is still in the request queue: the
 * flood placed no more than its limit
 */
TEST_F(DatagramSorterFixture, KeepsRoomForTheRequestOfAnotherStranger) {
  sort(otherStranger(), requestDatagram());

  sortRepeatedly(stranger(), requestDatagram(), FLOODING_REQUEST_COUNT);

  const std::vector<IncomingDatagram> requests = drainRequests();
  EXPECT_NE(
      std::ranges::find(requests, otherStranger(), &IncomingDatagram::sender),
      requests.end());
}

/**
 * Given an admitted player, and a forger that floods connection requests
 * without a session token from the address of that player
 * When the player sends a heartbeat with its token
 * Then the heartbeat reaches the player queue: the forger spent nothing of
 * the player's allowance
 */
TEST_F(DatagramSorterFixture, KeepsAPlayersDatagramsWhenAForgerUsesItsAddress) {
  admit(player());
  sortRepeatedly(player(), requestDatagram(), FLOODING_REQUEST_COUNT);

  sort(player(), heartbeatDatagram(playerHeader()));

  EXPECT_EQ(drainPlayers().size(), 1);
}

/**
 * Given an admitted player
 * When it sends sixty-one heartbeats at once with its token
 * Then sixty reach the player queue: the last is over its limit
 */
TEST_F(DatagramSorterFixture,
       PlacesNoMoreThanASecondOfAPlayersDatagramsAtOnce) {
  admit(player());

  sortRepeatedly(player(), heartbeatDatagram(playerHeader()),
                 PLAYER_DATAGRAMS_PLACED_AT_ONCE + 1);

  EXPECT_EQ(drainPlayers().size(), PLAYER_DATAGRAMS_PLACED_AT_ONCE);
}

/**
 * Given an admitted player banned for a flood with its own token
 * When it sends a heartbeat a nanosecond before the end of the ban
 * Then the heartbeat reaches no queue
 */
TEST_F(DatagramSorterFixture, DropsTheDatagramsOfABannedPlayer) {
  admit(player());
  sortRepeatedly(player(), heartbeatDatagram(playerHeader()),
                 PLAYER_FLOOD_TO_A_BAN);
  static_cast<void>(drainPlayers());

  advanceClock(JUST_BEFORE_THE_BAN_ENDS);
  sort(player(), heartbeatDatagram(playerHeader()));

  EXPECT_TRUE(drainPlayers().empty());
}

/**
 * Given an admitted player banned for a flood with its own token
 * When it sends a heartbeat once the ban ended
 * Then the heartbeat reaches the player queue
 */
TEST_F(DatagramSorterFixture, LetsABannedPlayerBackOnceTheBanEnds) {
  admit(player());
  sortRepeatedly(player(), heartbeatDatagram(playerHeader()),
                 PLAYER_FLOOD_TO_A_BAN);
  static_cast<void>(drainPlayers());

  advanceClock(PROTOCOL_BAN_DURATION);
  sort(player(), heartbeatDatagram(playerHeader()));

  EXPECT_EQ(drainPlayers().size(), 1);
}

/**
 * Given an admitted player banned for a flood with its own token
 * When it is admitted again with the same token
 * Then its next heartbeat is still dropped: admitting one session twice never
 * lifts its ban
 */
TEST_F(DatagramSorterFixture, KeepsTheBanOfAPlayerAdmittedAgainWithItsToken) {
  admit(player());
  sortRepeatedly(player(), heartbeatDatagram(playerHeader()),
                 PLAYER_FLOOD_TO_A_BAN);
  static_cast<void>(drainPlayers());

  admit(player());
  sort(player(), heartbeatDatagram(playerHeader()));

  EXPECT_TRUE(drainPlayers().empty());
}

/**
 * Given a sorter that admitted nobody
 * When a stranger sends twelve connection requests of another version at once
 * Then eleven reach the request queue: the refusals are limited like the
 * requests
 */
TEST_F(DatagramSorterFixture, LimitsTheDatagramsOfAnotherVersionLikeRequests) {
  rtype::network::PacketHeader header;
  header.version = NEXT_PROTOCOL_VERSION;

  sortRepeatedly(stranger(), requestDatagram(header),
                 REQUESTS_PLACED_AT_ONCE + 1);

  EXPECT_EQ(drainRequests().size(), REQUESTS_PLACED_AT_ONCE);
}

/**
 * Given a sorter that admitted nobody
 * When a stranger sends one connection request more than its limit
 * Then the journal holds one line about the datagram dropped, which says it
 * went over the rate limit of its sender
 */
TEST_F(DatagramSorterLogFixture, LogsWhyItDropsADatagramOverTheLimit) {
  sortRepeatedly(stranger(), requestDatagram(), REQUESTS_PLACED_AT_ONCE + 1);

  const std::vector<std::string> lines = dropLines();
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_NE(lines.front().find(rtype::server::OVER_RATE_LIMIT_DROP_REASON),
            std::string::npos);
}
