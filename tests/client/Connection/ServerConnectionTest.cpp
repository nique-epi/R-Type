#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
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
#include "Disconnection.hpp"
#include "Endpoint.hpp"
#include "GameStarted.hpp"
#include "IncomingDatagram.hpp"
#include "LobbyState.hpp"
#include "MessageFrame.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ReadyState.hpp"
#include "RecordingDatagramLink.hpp"
#include "ServerConnection.hpp"
#include "ServerConnectionTestConstants.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateCodec.hpp"

namespace rtype::client {
namespace {

using testing::ACCEPTANCE_SEQUENCE;
using testing::ACCEPTED_PLAYER_NUMBER;
using testing::CHALLENGE_COOKIE;
using ::testing::ElementsAre;
using ::testing::ElementsAreArray;
using testing::FIRST_SEQUENCE;
using testing::NEXT_PROTOCOL_VERSION;
using testing::NICKNAME;
using testing::ONE_NANOSECOND;
using testing::OTHER_NICKNAME;
using testing::OTHER_PLAYER_NUMBER;
using testing::OTHER_SENDER_PORT;
using testing::OTHER_SESSION_TOKEN;
using testing::PROTOCOL_ATTEMPT_TIMEOUT;
using testing::PROTOCOL_DISCONNECTION_COPY_COUNT;
using testing::PROTOCOL_DISCONNECTION_COPY_INTERVAL;
using testing::PROTOCOL_READY_STATE_INTERVAL;
using testing::PROTOCOL_REQUEST_INTERVAL;
using testing::SECOND_SEQUENCE;
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

/** @returns The message of type Message that @p datagram holds, its only
 * one. */
template <typename Message>
Message onlyMessageOf(const network::OutgoingDatagram& datagram) {
  const std::vector<network::MessageFrame> frames =
      network::DatagramCodec::splitMessages(datagram.payload);
  EXPECT_EQ(frames.size(), 1);
  EXPECT_EQ(frames.front().type, static_cast<std::uint8_t>(Message::TYPE));
  return network::decodeMessage<Message>(frames.front().payload);
}

/** @returns Whether @p datagram holds a message of type Message first. */
template <typename Message>
bool holds(const network::OutgoingDatagram& datagram) {
  const std::vector<network::MessageFrame> frames =
      network::DatagramCodec::splitMessages(datagram.payload);
  return !frames.empty() &&
         frames.front().type == static_cast<std::uint8_t>(Message::TYPE);
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
    return onlyMessageOf<network::ConnectionRequest>(link_.sent().back());
  }

  /** @brief Has the server accept the attempt as ACCEPTED_PLAYER_NUMBER,
   * with SESSION_TOKEN. */
  void accept() {
    connect();
    deliverInSession(
        network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER},
        ACCEPTANCE_SEQUENCE);
    update();
  }

  /** @brief Delivers @p message in a datagram of the server carrying this
   * sequence and SESSION_TOKEN. */
  template <typename Message>
  void deliverInSession(const Message& message, std::uint16_t sequence) {
    deliver(message, server(), sessionHeader(sequence));
  }

  /** @brief Delivers a WorldState of tick 1 with nobody in it, in a datagram
   * of the session carrying this sequence. */
  void deliverWorldStateInSession(std::uint16_t sequence) {
    const std::vector<std::vector<std::byte>> payloads =
        network::WorldStateCodec::encode(
            network::WorldState{.tick = 1, .players = {}, .entities = {}});
    deliverBytes(network::DatagramCodec::encode(
        sessionHeader(sequence), network::MessageType::WorldState,
        payloads.front()));
  }

  /** @returns How many datagrams sent so far hold a message of type
   * Message. */
  template <typename Message>
  [[nodiscard]] std::size_t sentCountOf() const {
    return static_cast<std::size_t>(
        std::ranges::count_if(link_.sent(), holds<Message>));
  }

  /** @returns A header of the server carrying this sequence and
   * SESSION_TOKEN. */
  static network::PacketHeader sessionHeader(std::uint16_t sequence) {
    network::PacketHeader header;
    header.sessionToken = SESSION_TOKEN;
    header.sequence = sequence;
    return header;
  }

 private:
  RecordingDatagramLink link_;
  ServerConnection connection_{link_};
};

/**
 * Given the delays of the protocol page
 * When the connection's request interval and timeout are read
 * Then they are 500 ms and 5 s
 */
TEST(ServerConnectionDelays, AreThoseOfTheProtocolPage) {
  EXPECT_EQ(CONNECTION_REQUEST_INTERVAL, PROTOCOL_REQUEST_INTERVAL);
  EXPECT_EQ(CONNECTION_ATTEMPT_TIMEOUT, PROTOCOL_ATTEMPT_TIMEOUT);
}

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

/**
 * Given the repetitions of the protocol page
 * When the connection's ReadyState interval and Disconnection copies are read
 * Then they are 500 ms, three copies, and 50 ms
 */
TEST(ServerConnectionDelays, SessionRepetitionsAreThoseOfTheProtocolPage) {
  EXPECT_EQ(READY_STATE_INTERVAL, PROTOCOL_READY_STATE_INTERVAL);
  EXPECT_EQ(DISCONNECTION_COPY_COUNT, PROTOCOL_DISCONNECTION_COPY_COUNT);
  EXPECT_EQ(DISCONNECTION_COPY_INTERVAL, PROTOCOL_DISCONNECTION_COPY_INTERVAL);
}

/**
 * Given an attempt the server just accepted as player 3
 * When the waiting room is read before any LobbyState
 * Then it holds player 3 alone, not ready, with the nickname it connected with
 */
TEST_F(ServerConnectionTest, TheRoomHoldsThePlayerAloneOnceAccepted) {
  accept();

  EXPECT_THAT(
      connection().lobbyPlayers(),
      ElementsAre(network::LobbyPlayer{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                                       .ready = false,
                                       .nickname = std::string{NICKNAME}}));
}

/**
 * Given a session the server opened with its token
 * When the player asks to be ready
 * Then a ReadyState saying ready goes at once, carrying the token
 */
TEST_F(ServerConnectionTest, SendsReadyStateAtOnceWithTheSessionToken) {
  accept();

  connection().setReady(true);

  ASSERT_TRUE(holds<network::ReadyState>(sent().back()));
  EXPECT_EQ(onlyMessageOf<network::ReadyState>(sent().back()),
            network::ReadyState{.ready = true});
  EXPECT_EQ(
      network::DatagramCodec::decodeHeader(sent().back().payload).sessionToken,
      SESSION_TOKEN);
}

/**
 * Given an attempt the server has not answered
 * When the player asks to be ready
 * Then nothing is sent and nothing is asked
 */
TEST_F(ServerConnectionTest, IgnoresReadyBeforeTheAcceptance) {
  connect();

  connection().setReady(true);

  EXPECT_EQ(sentCountOf<network::ReadyState>(), 0U);
  EXPECT_EQ(connection().requestedReady(), std::nullopt);
}

/**
 * Given a player who asked to be ready, whom the room still shows not ready
 * When one interval minus one nanosecond passes
 * Then the ReadyState is not sent again yet
 */
TEST_F(ServerConnectionTest, WaitsOneIntervalBeforeSendingReadyStateAgain) {
  accept();
  connection().setReady(true);

  update(READY_STATE_INTERVAL - ONE_NANOSECOND);

  EXPECT_EQ(sentCountOf<network::ReadyState>(), 1U);
}

/**
 * Given a player who asked to be ready, whom the room still shows not ready
 * When one interval passes
 * Then the ReadyState is sent again, since UDP may have lost it
 */
TEST_F(ServerConnectionTest, SendsReadyStateAgainWhileTheRoomShowsOther) {
  accept();
  connection().setReady(true);

  update(READY_STATE_INTERVAL);

  EXPECT_EQ(sentCountOf<network::ReadyState>(), 2U);
}

/**
 * Given a player who asked to be ready
 * When a LobbyState shows the player ready, then two intervals pass
 * Then the ReadyState is not sent again
 */
TEST_F(ServerConnectionTest, StopsSendingReadyStateOnceTheRoomShowsIt) {
  accept();
  connection().setReady(true);
  deliverInSession(
      network::LobbyState{.players = {{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                                       .ready = true,
                                       .nickname = std::string{NICKNAME}}}},
      FIRST_SEQUENCE);
  update();

  update(2 * READY_STATE_INTERVAL);

  EXPECT_EQ(sentCountOf<network::ReadyState>(), 1U);
}

/**
 * Given a session
 * When the server sends a LobbyState listing two players
 * Then the waiting room is those two players
 */
TEST_F(ServerConnectionTest, KeepsTheLobbyStateOfTheSession) {
  accept();
  const network::LobbyState lobby{
      .players = {{.playerNumber = OTHER_PLAYER_NUMBER,
                   .ready = true,
                   .nickname = std::string{OTHER_NICKNAME}},
                  {.playerNumber = ACCEPTED_PLAYER_NUMBER,
                   .ready = false,
                   .nickname = std::string{NICKNAME}}}};
  deliverInSession(lobby, FIRST_SEQUENCE);

  update();

  EXPECT_THAT(connection().lobbyPlayers(), ElementsAreArray(lobby.players));
}

/**
 * Given a session
 * When a LobbyState arrives with another token
 * Then the waiting room still holds the player alone
 */
TEST_F(ServerConnectionTest, IgnoresALobbyStateWithAnotherToken) {
  accept();
  network::PacketHeader forged = sessionHeader(FIRST_SEQUENCE);
  forged.sessionToken = OTHER_SESSION_TOKEN;
  deliver(network::LobbyState{.players = {{.playerNumber = OTHER_PLAYER_NUMBER,
                                           .ready = true,
                                           .nickname =
                                               std::string{OTHER_NICKNAME}}}},
          server(), forged);

  update();

  ASSERT_EQ(connection().lobbyPlayers().size(), 1U);
  EXPECT_EQ(connection().lobbyPlayers().front().playerNumber,
            ACCEPTED_PLAYER_NUMBER);
}

/**
 * Given a session that read the LobbyState of sequence 2
 * When the LobbyState of sequence 1 arrives late
 * Then the waiting room is still the one of sequence 2
 */
TEST_F(ServerConnectionTest, IgnoresALateLobbyState) {
  accept();
  const network::LobbyState newer{
      .players = {{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                   .ready = true,
                   .nickname = std::string{NICKNAME}}}};
  const network::LobbyState older{
      .players = {{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                   .ready = false,
                   .nickname = std::string{NICKNAME}}}};
  deliverInSession(newer, SECOND_SEQUENCE);
  deliverInSession(older, FIRST_SEQUENCE);

  update();

  EXPECT_THAT(connection().lobbyPlayers(), ElementsAreArray(newer.players));
}

/**
 * Given a session
 * When the server announces the start of the game
 * Then the connection says the game started
 */
TEST_F(ServerConnectionTest, LearnsThatTheGameStarted) {
  accept();
  deliverInSession(network::GameStarted{}, FIRST_SEQUENCE);

  update();

  EXPECT_TRUE(connection().gameStarted());
}

/**
 * Given a session whose GameStarted was lost
 * When the first WorldState arrives
 * Then the connection says the game started
 */
TEST_F(ServerConnectionTest, AWorldStateStartsTheGameWhenGameStartedWasLost) {
  accept();
  deliverWorldStateInSession(FIRST_SEQUENCE);

  update();

  EXPECT_TRUE(connection().gameStarted());
}

/**
 * Given a player who asked to be ready
 * When the game starts and an interval passes
 * Then the request is forgotten and no ReadyState goes out again
 */
TEST_F(ServerConnectionTest, ForgetsTheReadyRequestOnceTheGameStarted) {
  accept();
  connection().setReady(true);
  deliverInSession(network::GameStarted{}, FIRST_SEQUENCE);
  update();

  update(READY_STATE_INTERVAL);

  EXPECT_EQ(connection().requestedReady(), std::nullopt);
  EXPECT_EQ(sentCountOf<network::ReadyState>(), 1U);
}

/**
 * Given a session
 * When the player leaves it, then time passes, step by step
 * Then a Disconnection goes at once, then every 50 ms, three in all
 */
TEST_F(ServerConnectionTest, LeavingSendsThreeDisconnections50MsApart) {
  accept();

  connection().cancel();
  EXPECT_EQ(sentCountOf<network::Disconnection>(), 1U);
  update(DISCONNECTION_COPY_INTERVAL - ONE_NANOSECOND);
  EXPECT_EQ(sentCountOf<network::Disconnection>(), 1U);
  update(ONE_NANOSECOND);
  EXPECT_EQ(sentCountOf<network::Disconnection>(), 2U);
  update(DISCONNECTION_COPY_INTERVAL);
  EXPECT_EQ(sentCountOf<network::Disconnection>(), 3U);
  update(DISCONNECTION_COPY_INTERVAL);
  EXPECT_EQ(sentCountOf<network::Disconnection>(), 3U);
}

/**
 * Given a session
 * When the player leaves it
 * Then the Disconnection goes to that server, with the token of the session
 */
TEST_F(ServerConnectionTest, TheDisconnectionCarriesTheTokenOfTheSession) {
  accept();

  connection().cancel();

  ASSERT_TRUE(holds<network::Disconnection>(sent().back()));
  EXPECT_EQ(sent().back().destination, server());
  EXPECT_EQ(
      network::DatagramCodec::decodeHeader(sent().back().payload).sessionToken,
      SESSION_TOKEN);
}

/**
 * Given a session with a room, a request and a started game
 * When the player leaves it
 * Then the room, the request and the start are forgotten
 */
TEST_F(ServerConnectionTest, LeavingForgetsTheSession) {
  accept();
  connection().setReady(true);
  deliverInSession(network::GameStarted{}, FIRST_SEQUENCE);
  update();

  connection().cancel();

  EXPECT_TRUE(connection().lobbyPlayers().empty());
  EXPECT_EQ(connection().requestedReady(), std::nullopt);
  EXPECT_FALSE(connection().gameStarted());
}

/**
 * Given a session
 * When the client connects again
 * Then a Disconnection to the session left goes before the new request
 */
TEST_F(ServerConnectionTest, ConnectingAgainLeavesTheSessionFirst) {
  accept();

  connect();

  ASSERT_GE(sent().size(), 2U);
  EXPECT_TRUE(holds<network::Disconnection>(sent().at(sent().size() - 2)));
  EXPECT_TRUE(holds<network::ConnectionRequest>(sent().back()));
}

}  // namespace
}  // namespace rtype::client
