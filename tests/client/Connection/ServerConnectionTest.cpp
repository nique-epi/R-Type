#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include "ConnectionAccepted.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionConstants.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionRequest.hpp"
#include "ConnectionStatus.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "MessageFrame.hpp"
#include "MessagePayload.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "RecordingDatagramLink.hpp"
#include "ServerConnection.hpp"
#include "ServerConnectionTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {
namespace {

using testing::ACCEPTED_PLAYER_NUMBER;
using testing::CHALLENGE_COOKIE;
using testing::NEXT_PROTOCOL_VERSION;
using testing::NICKNAME;
using testing::ONE_NANOSECOND;
using testing::OTHER_SENDER_PORT;
using testing::SERVER_ADDRESS;
using testing::SERVER_PORT;
using testing::SESSION_TOKEN;

using Phase = ConnectionStatus::Phase;

network::Endpoint server() {
  return network::Endpoint{.address = std::string{SERVER_ADDRESS},
                           .port = SERVER_PORT};
}

network::Endpoint otherSender() {
  return network::Endpoint{.address = std::string{SERVER_ADDRESS},
                           .port = OTHER_SENDER_PORT};
}

/** @returns The request @p datagram holds, its only message. */
network::ConnectionRequest requestOf(
    const network::OutgoingDatagram& datagram) {
  const std::vector<network::MessageFrame> frames =
      network::DatagramCodec::splitMessages(datagram.payload);
  EXPECT_EQ(frames.size(), 1);
  return network::decodeMessage<network::ConnectionRequest>(
      frames.front().payload);
}

/** @brief A connection whose datagrams go through a recording link. */
class ServerConnectionTest : public ::testing::Test {
 protected:
  void connect() { connection_.connect(server(), NICKNAME); }

  /** @brief Delivers a datagram holding @p message, as @p sender sent it. */
  template <typename Message>
  void deliver(const Message& message,
               const network::Endpoint& sender = server(),
               network::PacketHeader header = {}) {
    link_.deliver(network::IncomingDatagram{
        .sender = sender,
        .payload = network::DatagramCodec::encode(header, message)});
  }

  void deliverBytes(std::vector<std::byte> datagram) {
    link_.deliver(network::IncomingDatagram{.sender = server(),
                                            .payload = std::move(datagram)});
  }

  void update(engine::Duration elapsed = engine::Duration::zero()) {
    connection_.update(elapsed);
  }

  [[nodiscard]] Phase phase() const { return connection_.status().phase; }

  [[nodiscard]] ServerConnection& connection() { return connection_; }

  [[nodiscard]] const std::vector<network::OutgoingDatagram>& sent() const {
    return link_.sent();
  }

  [[nodiscard]] network::ConnectionRequest lastRequest() const {
    return requestOf(link_.sent().back());
  }

 private:
  RecordingDatagramLink link_;
  ServerConnection connection_{link_};
};

/**
 * Given a connection that never started an attempt
 * When its status is read
 * Then there is no attempt
 */
TEST_F(ServerConnectionTest, StartsWithNoAttempt) {
  EXPECT_EQ(phase(), Phase::Idle);
}

/**
 * Given a connection with no attempt
 * When it connects to a server
 * Then a request without a cookie, carrying the nickname, goes to that server
 * at once
 */
TEST_F(ServerConnectionTest, SendsARequestWithoutCookieAtOnce) {
  connect();

  ASSERT_EQ(sent().size(), 1);
  EXPECT_EQ(sent().front().destination, server());
  EXPECT_EQ(lastRequest(),
            (network::ConnectionRequest{.cookie = 0,
                                        .nickname = std::string{NICKNAME}}));
  EXPECT_EQ(phase(), Phase::Connecting);
}

/**
 * Given an attempt that just started
 * When one request interval minus one nanosecond passes without an answer
 * Then the request is not sent again yet
 */
TEST_F(ServerConnectionTest, WaitsOneIntervalBeforeSendingAgain) {
  connect();

  update(CONNECTION_REQUEST_INTERVAL - ONE_NANOSECOND);

  EXPECT_EQ(sent().size(), 1);
}

/**
 * Given an attempt that just started
 * When one request interval passes without an answer
 * Then the request is sent again, since UDP may have lost it
 */
TEST_F(ServerConnectionTest, SendsTheRequestAgainAfterOneInterval) {
  connect();

  update(CONNECTION_REQUEST_INTERVAL);

  EXPECT_EQ(sent().size(), 2);
}

/**
 * Given an attempt that sent its request twice
 * When the headers of the two requests are read
 * Then they carry the sequences 0 and 1
 */
TEST_F(ServerConnectionTest, NumbersEachRequestWithTheNextSequence) {
  connect();
  update(CONNECTION_REQUEST_INTERVAL);

  ASSERT_EQ(sent().size(), 2);
  EXPECT_EQ(network::DatagramCodec::decodeHeader(sent().at(0).payload).sequence,
            0);
  EXPECT_EQ(network::DatagramCodec::decodeHeader(sent().at(1).payload).sequence,
            1);
}

/**
 * Given an attempt in progress
 * When the server answers with a challenge
 * Then the request goes again at once, carrying the cookie of the challenge
 */
TEST_F(ServerConnectionTest, AnswersAChallengeAtOnceWithItsCookie) {
  connect();
  deliver(network::ConnectionChallenge{.cookie = CHALLENGE_COOKIE});

  update();

  ASSERT_EQ(sent().size(), 2);
  EXPECT_EQ(lastRequest().cookie, CHALLENGE_COOKIE);
}

/**
 * Given an attempt that answered a challenge
 * When one more interval passes without an answer
 * Then the request sent again still carries the cookie
 */
TEST_F(ServerConnectionTest, KeepsTheCookieForTheRequestsThatFollow) {
  connect();
  deliver(network::ConnectionChallenge{.cookie = CHALLENGE_COOKIE});
  update();

  update(CONNECTION_REQUEST_INTERVAL);

  ASSERT_EQ(sent().size(), 3);
  EXPECT_EQ(lastRequest().cookie, CHALLENGE_COOKIE);
}

/**
 * Given an attempt in progress
 * When the server accepts it as player 3
 * Then the status says accepted as player 3
 */
TEST_F(ServerConnectionTest, IsAcceptedWithThePlayerNumberOfTheServer) {
  connect();
  network::PacketHeader header;
  header.sessionToken = SESSION_TOKEN;
  deliver(network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER},
          server(), header);

  update();

  EXPECT_EQ(connection().status(),
            (ConnectionStatus{.phase = Phase::Accepted,
                              .playerNumber = ACCEPTED_PLAYER_NUMBER}));
}

/**
 * Given an attempt in progress
 * When the server refuses it because the game is full
 * Then the status says refused because the game is full
 */
TEST_F(ServerConnectionTest, IsRefusedWithTheReasonOfTheServer) {
  connect();
  deliver(
      network::ConnectionRefused{.reason = network::RefusalReason::GameFull});

  update();

  EXPECT_EQ(connection().status(),
            (ConnectionStatus{.phase = Phase::Refused,
                              .refusal = network::RefusalReason::GameFull}));
}

/**
 * Given an attempt the server accepted
 * When two more intervals pass
 * Then no request is sent again
 */
TEST_F(ServerConnectionTest, SendsNothingOnceAccepted) {
  connect();
  deliver(network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER});
  update();

  update(2 * CONNECTION_REQUEST_INTERVAL);

  EXPECT_EQ(sent().size(), 1);
}

/**
 * Given an attempt with no answer
 * When the attempt timeout minus one nanosecond has passed
 * Then it is still connecting
 */
TEST_F(ServerConnectionTest, KeepsTryingUntilTheTimeout) {
  connect();

  update(CONNECTION_ATTEMPT_TIMEOUT - ONE_NANOSECOND);

  EXPECT_EQ(phase(), Phase::Connecting);
}

/**
 * Given an attempt with no answer
 * When the attempt timeout has passed
 * Then the server is unreachable
 */
TEST_F(ServerConnectionTest, GivesUpAtTheTimeout) {
  connect();

  update(CONNECTION_ATTEMPT_TIMEOUT);

  EXPECT_EQ(phase(), Phase::Unreachable);
}

/**
 * Given an attempt that gave up
 * When another interval passes
 * Then no request is sent again
 */
TEST_F(ServerConnectionTest, SendsNothingOnceItGaveUp) {
  connect();
  update(CONNECTION_ATTEMPT_TIMEOUT);
  const std::size_t sentWhenGivingUp = sent().size();

  update(CONNECTION_REQUEST_INTERVAL);

  EXPECT_EQ(sent().size(), sentWhenGivingUp);
}

/**
 * Given an attempt challenged just before its timeout
 * When the rest of the timeout passes without acceptance or refusal
 * Then it gives up: a challenge does not restart the timeout
 */
TEST_F(ServerConnectionTest, ChallengeDoesNotRestartTheTimeout) {
  connect();
  update(CONNECTION_ATTEMPT_TIMEOUT - ONE_NANOSECOND);
  deliver(network::ConnectionChallenge{.cookie = CHALLENGE_COOKIE});
  update();

  update(ONE_NANOSECOND);

  EXPECT_EQ(phase(), Phase::Unreachable);
}

/**
 * Given an attempt in progress
 * When another sender on the server's address accepts it
 * Then the attempt is still connecting
 */
TEST_F(ServerConnectionTest, IgnoresAnAnswerFromAnotherSender) {
  connect();
  deliver(network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER},
          otherSender());

  update();

  EXPECT_EQ(phase(), Phase::Connecting);
}

/**
 * Given an attempt in progress
 * When the server's acceptance arrives with its last byte cut off
 * Then the attempt is still connecting, and nothing is thrown
 */
TEST_F(ServerConnectionTest, IgnoresATruncatedDatagram) {
  connect();
  std::vector<std::byte> datagram = network::DatagramCodec::encode(
      network::PacketHeader{},
      network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER});
  datagram.pop_back();
  deliverBytes(std::move(datagram));

  EXPECT_NO_THROW(update());
  EXPECT_EQ(phase(), Phase::Connecting);
}

/**
 * Given an attempt in progress
 * When an acceptance of another protocol version arrives
 * Then the attempt is still connecting
 */
TEST_F(ServerConnectionTest, IgnoresAnAnswerOfAnotherVersion) {
  connect();
  network::PacketHeader header;
  header.version = NEXT_PROTOCOL_VERSION;
  deliver(network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER},
          server(), header);

  update();

  EXPECT_EQ(phase(), Phase::Connecting);
}

/**
 * Given an attempt in progress
 * When it is cancelled and another interval passes
 * Then there is no attempt and no request is sent again
 */
TEST_F(ServerConnectionTest, SendsNothingOnceCancelled) {
  connect();

  connection().cancel();
  update(CONNECTION_REQUEST_INTERVAL);

  EXPECT_EQ(phase(), Phase::Idle);
  EXPECT_EQ(sent().size(), 1);
}

/**
 * Given an attempt that was cancelled
 * When the server's acceptance arrives late
 * Then there is still no attempt
 */
TEST_F(ServerConnectionTest, IgnoresALateAnswerOnceCancelled) {
  connect();
  connection().cancel();
  deliver(network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER});

  update();

  EXPECT_EQ(phase(), Phase::Idle);
}

/**
 * Given an attempt the server refused, whose acceptance was then delivered
 * late, unread
 * When the client connects again
 * Then the new attempt is connecting, unmoved by the stale answer
 */
TEST_F(ServerConnectionTest, StartsANewAttemptWithoutStaleAnswers) {
  connect();
  deliver(
      network::ConnectionRefused{.reason = network::RefusalReason::GameFull});
  update();
  deliver(network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER});

  connect();
  update();

  EXPECT_EQ(phase(), Phase::Connecting);
  EXPECT_EQ(lastRequest().cookie, 0);
}

}  // namespace
}  // namespace rtype::client
