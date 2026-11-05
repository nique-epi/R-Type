#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "DatagramCodec.hpp"
#include "EncodedMessage.hpp"
#include "IncomingDatagram.hpp"
#include "MessagePayload.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerDatagrams.hpp"
#include "ServerInformation.hpp"
#include "ServerInformationDeskFixture.hpp"
#include "ServerInformationDeskFixtures.hpp"
#include "ServerInformationDeskTestConstants.hpp"
#include "ServerInformationRequest.hpp"

using rtype::network::IncomingDatagram;
using rtype::network::ServerInformation;
using rtype::server::testing::ANSWERED_THREE_MILLISECONDS_LATER;
using rtype::server::testing::ASKED_AT;
using rtype::server::testing::DESK_GAME_VERSION;
using rtype::server::testing::DESK_OCCUPANCY;
using rtype::server::testing::DESK_SERVER_NAME;
using rtype::server::testing::LONGEST_SERVER_NAME;
using rtype::server::testing::onlyMessageIn;
using rtype::server::testing::playerEndpoint;
using rtype::server::testing::REQUEST_ARRIVAL;
using rtype::server::testing::THREE_MILLISECONDS_IN_MICROSECONDS;

/**
 * Given a desk of a server named Lyon, five players in two games and three
 * free seats
 * When a request arrives
 * Then its sender gets one ServerInformation that echoes its sentAt and says
 * all of that, with the protocol version this code speaks
 */
TEST_F(ServerInformationDeskFixture, AnswersTheSenderWithWhatTheServerIs) {
  answer({informationRequestArriving(ASKED_AT)}, REQUEST_ARRIVAL);

  ASSERT_EQ(sent().size(), 1U);
  EXPECT_EQ(sent().front().destination, playerEndpoint(0));
  EXPECT_EQ(
      onlyMessageIn<ServerInformation>(sent().front()),
      (ServerInformation{.sentAt = ASKED_AT,
                         .heldFor = 0,
                         .protocolVersion = rtype::network::PROTOCOL_VERSION,
                         .gameVersion = std::string{DESK_GAME_VERSION},
                         .name = std::string{DESK_SERVER_NAME},
                         .playerCount = DESK_OCCUPANCY.playerCount,
                         .gameCount = DESK_OCCUPANCY.gameCount,
                         .freeSeatCount = DESK_OCCUPANCY.freeSeatCount}));
}

/**
 * Given a request that arrived three milliseconds before the desk answers it
 * When the desk answers
 * Then heldFor is 3000 microseconds, so the asker leaves out the wait for the
 * tick
 */
TEST_F(ServerInformationDeskFixture, SaysHowLongItHeldTheRequest) {
  answer({informationRequestArriving(ASKED_AT)},
         ANSWERED_THREE_MILLISECONDS_LATER);

  ASSERT_EQ(sent().size(), 1U);
  EXPECT_EQ(onlyMessageIn<ServerInformation>(sent().front()).heldFor,
            THREE_MILLISECONDS_IN_MICROSECONDS);
}

/**
 * Given a desk of a server whose name takes the 32 bytes the protocol allows
 * When a request arrives
 * Then the answer is no larger than the request: a forged address gets no
 * more bytes than its forger sent
 */
TEST_F(ServerInformationDeskFixture, NeverAnswersMoreBytesThanItReceived) {
  answerAs(LONGEST_SERVER_NAME);
  const IncomingDatagram request = informationRequestArriving(ASKED_AT);

  answer({request}, REQUEST_ARRIVAL);

  ASSERT_EQ(sent().size(), 1U);
  EXPECT_LE(sent().front().payload.size(), request.payload.size());
}

/**
 * Given a request
 * When the desk answers it
 * Then the answer carries no session token and sequence 0: it is sent
 * outside any session
 */
TEST_F(ServerInformationDeskFixture, AnswersOutsideAnySession) {
  answer({informationRequestArriving(ASKED_AT)}, REQUEST_ARRIVAL);

  ASSERT_EQ(sent().size(), 1U);
  EXPECT_EQ(rtype::server::testing::headerOf(sent().front()),
            rtype::network::PacketHeader{});
}

/**
 * Given a request whose padding lacks its last byte
 * When it arrives
 * Then nothing is sent: a request too short to pay for its answer is ignored
 */
TEST_F(ServerInformationDeskFixture, IgnoresARequestTooShort) {
  answer({cutShort(informationRequestArriving(ASKED_AT))}, REQUEST_ARRIVAL);

  EXPECT_TRUE(sent().empty());
}

/**
 * Given a datagram holding a Heartbeat and no request
 * When it arrives
 * Then nothing is sent
 */
TEST_F(ServerInformationDeskFixture, IgnoresADatagramWithoutARequest) {
  answer({IncomingDatagram{.sender = playerEndpoint(0),
                           .payload = heartbeatDatagram(),
                           .receivedAt = REQUEST_ARRIVAL}},
         REQUEST_ARRIVAL);

  EXPECT_TRUE(sent().empty());
}

/**
 * Given a datagram holding two requests
 * When it arrives
 * Then one answer is sent, to the first request
 */
TEST_F(ServerInformationDeskFixture, AnswersADatagramOnce) {
  const std::vector<rtype::network::EncodedMessage> requests{
      rtype::network::encodedMessage(
          rtype::network::ServerInformationRequest{.sentAt = ASKED_AT}),
      rtype::network::encodedMessage(
          rtype::network::ServerInformationRequest{.sentAt = 0})};

  answer({IncomingDatagram{.sender = playerEndpoint(0),
                           .payload = rtype::network::DatagramCodec::encode(
                               rtype::network::PacketHeader{}, requests),
                           .receivedAt = REQUEST_ARRIVAL}},
         REQUEST_ARRIVAL);

  ASSERT_EQ(sent().size(), 1U);
  EXPECT_EQ(onlyMessageIn<ServerInformation>(sent().front()).sentAt, ASKED_AT);
}
