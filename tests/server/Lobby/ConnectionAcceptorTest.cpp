#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "ByteWriter.hpp"
#include "ConnectedPlayer.hpp"
#include "ConnectionAccepted.hpp"
#include "ConnectionAcceptor.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "MessageFrame.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "ReadyState.hpp"
#include "ServerDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {
namespace {

using testing::COOKIE_EXPIRED_TIME;
using testing::FIVE_PLAYER_PORTS;
using testing::NEXT_PROTOCOL_VERSION;
using testing::OTHER_PROTOCOL_ID;
using testing::PLAYER_ADDRESS;
using testing::REQUEST_TIME;
using testing::SHORTEST_NICKNAME;

network::Endpoint playerEndpoint(std::size_t player) {
  return network::Endpoint{.address = std::string{PLAYER_ADDRESS},
                           .port = FIVE_PLAYER_PORTS.at(player)};
}

/** @returns The type of the only message of @p datagram. */
std::uint8_t messageTypeOf(const network::OutgoingDatagram& datagram) {
  const std::vector<network::MessageFrame> frames =
      network::DatagramCodec::splitMessages(datagram.payload);
  EXPECT_EQ(frames.size(), 1);
  return frames.front().type;
}

/** @returns The only message of @p datagram, read as a Message. */
template <typename Message>
Message messageOf(const network::OutgoingDatagram& datagram) {
  EXPECT_EQ(messageTypeOf(datagram), static_cast<std::uint8_t>(Message::TYPE));
  return network::decodeMessage<Message>(
      network::DatagramCodec::splitMessages(datagram.payload).front().payload);
}

/** @brief An acceptor whose answers are recorded instead of sent. */
class ConnectionAcceptorTest : public ::testing::Test {
 protected:
  void receive(const network::Endpoint& sender, std::vector<std::byte> datagram,
               engine::Duration now = REQUEST_TIME) {
    const network::IncomingDatagram received{.sender = sender,
                                             .payload = std::move(datagram)};
    acceptor_.receive(std::span{&received, 1}, now);
  }

  /** @returns The cookie of the challenge @p sender gets for a request
   * without one. */
  std::uint64_t challengeCookie(const network::Endpoint& sender) {
    receive(sender, requestDatagram(0));
    return messageOf<network::ConnectionChallenge>(lastAnswer()).cookie;
  }

  /** @brief Goes through the challenge and sends the request with its
   * cookie. */
  void connect(const network::Endpoint& sender) {
    receive(sender, requestDatagram(challengeCookie(sender)));
  }

  [[nodiscard]] const std::vector<network::OutgoingDatagram>& answers() const {
    return answers_;
  }

  [[nodiscard]] const network::OutgoingDatagram& lastAnswer() const {
    return answers_.back();
  }

  [[nodiscard]] PlayerRoster& roster() { return roster_; }

  [[nodiscard]] const std::vector<network::Endpoint>& admitted() const {
    return admitted_;
  }

  [[nodiscard]] ConnectionAcceptor& acceptor() { return acceptor_; }

  /** @returns The numbers of the players the acceptor reported as joined. */
  [[nodiscard]] const std::vector<std::uint8_t>& joinedPlayers() const {
    return joinedPlayers_;
  }

 private:
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> answers_;
  std::vector<network::Endpoint> admitted_;
  std::vector<std::uint8_t> joinedPlayers_;
  ConnectionAcceptor acceptor_{
      roster_,
      [this](network::OutgoingDatagram datagram) {
        answers_.push_back(std::move(datagram));
      },
      [this](const network::Endpoint& player) { admitted_.push_back(player); },
      [this](ConnectedPlayer& player) {
        joinedPlayers_.push_back(player.playerNumber);
      }};
};

/**
 * Given an acceptor with nobody connected
 * When a sender goes through the challenge, then repeats its request
 * Then the acceptor reports the player as joined once, not at the repeat
 */
TEST_F(ConnectionAcceptorTest, ReportsAJoinedPlayerOnlyOnce) {
  const std::uint64_t cookie = challengeCookie(playerEndpoint(0));
  receive(playerEndpoint(0), requestDatagram(cookie));
  receive(playerEndpoint(0), requestDatagram(cookie));

  EXPECT_EQ(joinedPlayers(), (std::vector<std::uint8_t>{1}));
}

/**
 * Given an acceptor with nobody connected
 * When a sender requests a connection without a cookie
 * Then no player is reported as joined: a challenge reserves nothing
 */
TEST_F(ConnectionAcceptorTest, ReportsNobodyForAChallengedSender) {
  receive(playerEndpoint(0), requestDatagram(0));

  EXPECT_TRUE(joinedPlayers().empty());
}

/**
 * Given an acceptor with nobody connected
 * When a sender requests a connection without a cookie
 * Then it gets a challenge with a cookie, sent back to its address
 */
TEST_F(ConnectionAcceptorTest, ChallengesARequestWithoutCookie) {
  receive(playerEndpoint(0), requestDatagram(0));

  ASSERT_EQ(answers().size(), 1);
  EXPECT_EQ(lastAnswer().destination, playerEndpoint(0));
  EXPECT_NE(messageOf<network::ConnectionChallenge>(lastAnswer()).cookie, 0);
}

/**
 * Given an acceptor with nobody connected
 * When a sender requests a connection without a cookie
 * Then nobody takes a place: a challenge reserves nothing
 */
TEST_F(ConnectionAcceptorTest, KeepsNothingForAChallengedSender) {
  receive(playerEndpoint(0), requestDatagram(0));

  EXPECT_EQ(roster().find(playerEndpoint(0)), nullptr);
}

/**
 * Given a sender that requested a connection with the smallest request
 * When the acceptor answers it
 * Then the answer holds no more bytes than the request did
 */
TEST_F(ConnectionAcceptorTest, NeverAnswersAnUnknownSenderWithMoreBytes) {
  const std::vector<std::byte> request = network::DatagramCodec::encode(
      network::PacketHeader{},
      network::ConnectionRequest{.cookie = 0,
                                 .nickname = std::string{SHORTEST_NICKNAME}});

  receive(playerEndpoint(0), request);

  ASSERT_EQ(answers().size(), 1);
  EXPECT_LE(lastAnswer().payload.size(), request.size());
}

/**
 * Given a sender that got a challenge
 * When it sends its request again with the cookie of the challenge
 * Then it is accepted as player 1, under a session token other than 0
 */
TEST_F(ConnectionAcceptorTest, AcceptsARequestCarryingItsCookie) {
  connect(playerEndpoint(0));

  EXPECT_EQ(messageOf<network::ConnectionAccepted>(lastAnswer()).playerNumber,
            network::FIRST_PLAYER_NUMBER);
  EXPECT_NE(
      network::DatagramCodec::decodeHeader(lastAnswer().payload).sessionToken,
      0);
}

/**
 * Given a sender that got a challenge
 * When it sends its request again with the cookie of the challenge
 * Then its address is announced as admitted, once
 */
TEST_F(ConnectionAcceptorTest, AnnouncesTheAddressOfANewPlayer) {
  connect(playerEndpoint(0));

  EXPECT_EQ(admitted(), (std::vector<network::Endpoint>{playerEndpoint(0)}));
}

/**
 * Given an acceptor with nobody connected
 * When a sender is challenged
 * Then nobody is announced as admitted
 */
TEST_F(ConnectionAcceptorTest, AnnouncesNobodyForAChallenge) {
  receive(playerEndpoint(0), requestDatagram(0));

  EXPECT_TRUE(admitted().empty());
}

/**
 * Given a connected player
 * When it is accepted again
 * Then it is not announced a second time
 */
TEST_F(ConnectionAcceptorTest, AnnouncesAPlayerAcceptedAgainOnlyOnce) {
  connect(playerEndpoint(0));

  connect(playerEndpoint(0));

  EXPECT_EQ(admitted().size(), 1);
}

/**
 * Given an acceptor with nobody connected
 * When four senders connect one after the other
 * Then they are accepted as players 1, 2, 3 and 4
 */
TEST_F(ConnectionAcceptorTest, NumbersFourPlayersInTheOrderTheyConnect) {
  for (std::size_t player = 0; player < network::MAXIMUM_PLAYERS; ++player) {
    connect(playerEndpoint(player));

    EXPECT_EQ(messageOf<network::ConnectionAccepted>(lastAnswer()).playerNumber,
              network::FIRST_PLAYER_NUMBER + player);
  }
}

/**
 * Given four connected players
 * When a fifth sender requests a connection, with no cookie yet
 * Then it is refused at once because the game is full
 */
TEST_F(ConnectionAcceptorTest, RefusesTheFifthPlayerBecauseTheGameIsFull) {
  for (std::size_t player = 0; player < network::MAXIMUM_PLAYERS; ++player) {
    connect(playerEndpoint(player));
  }

  receive(playerEndpoint(network::MAXIMUM_PLAYERS), requestDatagram(0));

  EXPECT_EQ(messageOf<network::ConnectionRefused>(lastAnswer()).reason,
            network::RefusalReason::GameFull);
}

/**
 * Given a game in progress
 * When a sender that is not connected requests a connection
 * Then it is refused because the game is in progress
 */
TEST_F(ConnectionAcceptorTest, RefusesANewPlayerWhileAGameIsInProgress) {
  acceptor().setGameInProgress(true);

  receive(playerEndpoint(0), requestDatagram(0));

  EXPECT_EQ(messageOf<network::ConnectionRefused>(lastAnswer()).reason,
            network::RefusalReason::GameInProgress);
}

/**
 * Given a game that was in progress and is no longer
 * When a sender requests a connection without a cookie
 * Then it is challenged, not refused
 */
TEST_F(ConnectionAcceptorTest, LetsPlayersInAgainOnceTheGameIsOver) {
  acceptor().setGameInProgress(true);
  acceptor().setGameInProgress(false);

  receive(playerEndpoint(0), requestDatagram(0));

  EXPECT_EQ(
      messageTypeOf(lastAnswer()),
      static_cast<std::uint8_t>(network::MessageType::ConnectionChallenge));
}

/**
 * Given a connected player whose acceptance was lost
 * When it sends its request with a valid cookie again
 * Then it gets the same number and the same session token again
 */
TEST_F(ConnectionAcceptorTest, AcceptsAConnectedPlayerAgainTheSameWay) {
  connect(playerEndpoint(0));
  const network::OutgoingDatagram first = lastAnswer();

  connect(playerEndpoint(0));

  EXPECT_EQ(messageOf<network::ConnectionAccepted>(lastAnswer()),
            messageOf<network::ConnectionAccepted>(first));
  EXPECT_EQ(
      network::DatagramCodec::decodeHeader(lastAnswer().payload).sessionToken,
      network::DatagramCodec::decodeHeader(first.payload).sessionToken);
}

/**
 * Given a connected player accepted once
 * When it is accepted a second time
 * Then the second acceptance carries the next sequence of the session
 */
TEST_F(ConnectionAcceptorTest, NumbersEachAcceptanceWithTheNextSequence) {
  connect(playerEndpoint(0));
  const std::uint16_t firstSequence =
      network::DatagramCodec::decodeHeader(lastAnswer().payload).sequence;

  connect(playerEndpoint(0));

  EXPECT_EQ(firstSequence, 0);
  EXPECT_EQ(network::DatagramCodec::decodeHeader(lastAnswer().payload).sequence,
            1);
}

/**
 * Given a connected player and a game that started since
 * When the player sends its request with a valid cookie again
 * Then it is accepted again, not refused: it already plays
 */
TEST_F(ConnectionAcceptorTest, AcceptsAConnectedPlayerDuringItsGame) {
  connect(playerEndpoint(0));
  acceptor().setGameInProgress(true);

  connect(playerEndpoint(0));

  EXPECT_EQ(messageOf<network::ConnectionAccepted>(lastAnswer()).playerNumber,
            network::FIRST_PLAYER_NUMBER);
}

/**
 * Given a connected player and a game that started since
 * When the player sends a request without a cookie
 * Then it gets a challenge, not a refusal: it already has its place
 */
TEST_F(ConnectionAcceptorTest,
       ChallengesAConnectedPlayerWithoutCookieDuringItsGame) {
  connect(playerEndpoint(0));
  acceptor().setGameInProgress(true);

  receive(playerEndpoint(0), requestDatagram(0));

  EXPECT_EQ(
      messageTypeOf(lastAnswer()),
      static_cast<std::uint8_t>(network::MessageType::ConnectionChallenge));
}

/**
 * Given a sender that got a challenge
 * When it sends a request with another cookie
 * Then it gets a new challenge and takes no place
 */
TEST_F(ConnectionAcceptorTest, ChallengesARequestWithAWrongCookie) {
  const std::uint64_t cookie = challengeCookie(playerEndpoint(0));

  receive(playerEndpoint(0), requestDatagram(cookie + 1));

  EXPECT_EQ(
      messageTypeOf(lastAnswer()),
      static_cast<std::uint8_t>(network::MessageType::ConnectionChallenge));
  EXPECT_EQ(roster().find(playerEndpoint(0)), nullptr);
}

/**
 * Given a sender that got a challenge
 * When it sends the cookie back two cookie windows later
 * Then the cookie has expired: it gets a new challenge
 */
TEST_F(ConnectionAcceptorTest, ChallengesARequestWithAnExpiredCookie) {
  const std::uint64_t cookie = challengeCookie(playerEndpoint(0));

  receive(playerEndpoint(0), requestDatagram(cookie), COOKIE_EXPIRED_TIME);

  EXPECT_EQ(
      messageTypeOf(lastAnswer()),
      static_cast<std::uint8_t>(network::MessageType::ConnectionChallenge));
}

/**
 * Given a cookie challenged to one sender
 * When another sender, as one forging an address would, sends it back
 * Then the other sender takes no place
 */
TEST_F(ConnectionAcceptorTest, RefusesThePlaceToASenderWithAnotherOnesCookie) {
  const std::uint64_t cookie = challengeCookie(playerEndpoint(0));

  receive(playerEndpoint(1), requestDatagram(cookie));

  EXPECT_EQ(roster().find(playerEndpoint(1)), nullptr);
}

/**
 * Given an acceptor with nobody connected
 * When an unknown sender sends a message other than a connection request
 * Then it gets no answer and takes no place
 */
TEST_F(ConnectionAcceptorTest, IgnoresAnythingButARequestFromAnUnknownSender) {
  receive(playerEndpoint(0),
          network::DatagramCodec::encode(network::PacketHeader{},
                                         network::ReadyState{.ready = true}));

  EXPECT_TRUE(answers().empty());
  EXPECT_EQ(roster().find(playerEndpoint(0)), nullptr);
}

/**
 * Given a datagram holding two connection requests
 * When the acceptor receives it
 * Then it answers once: a datagram never gets more than one answer
 */
TEST_F(ConnectionAcceptorTest, AnswersADatagramOnceWhateverItHolds) {
  std::vector<std::byte> datagram = requestDatagram(0);
  const std::vector<std::byte> secondMessage{
      datagram.begin() + network::PACKET_HEADER_SIZE, datagram.end()};
  datagram.insert(datagram.end(), secondMessage.begin(), secondMessage.end());

  receive(playerEndpoint(0), datagram);

  EXPECT_EQ(answers().size(), 1);
}

/**
 * Given a request datagram whose last byte was cut off
 * When the acceptor receives it
 * Then it gets no answer, and nothing is thrown
 */
TEST_F(ConnectionAcceptorTest, IgnoresATruncatedDatagram) {
  std::vector<std::byte> datagram = requestDatagram(0);
  datagram.pop_back();

  EXPECT_NO_THROW(receive(playerEndpoint(0), datagram));
  EXPECT_TRUE(answers().empty());
}

/**
 * Given a request whose nickname is empty, which the protocol forbids
 * When the acceptor receives it
 * Then it gets no answer, and nothing is thrown
 */
TEST_F(ConnectionAcceptorTest, IgnoresARequestWithAnInvalidNickname) {
  network::ByteWriter payload;
  payload.writeUint64(0);
  payload.writeString("");

  EXPECT_NO_THROW(receive(
      playerEndpoint(0),
      network::DatagramCodec::encode(network::PacketHeader{},
                                     network::MessageType::ConnectionRequest,
                                     payload.bytes())));
  EXPECT_TRUE(answers().empty());
}

/**
 * Given a request of the next protocol version
 * When the acceptor receives it
 * Then it gets no answer
 */
TEST_F(ConnectionAcceptorTest, IgnoresARequestOfAnotherVersion) {
  network::PacketHeader header;
  header.version = NEXT_PROTOCOL_VERSION;

  receive(playerEndpoint(0), requestDatagram(0, header));

  EXPECT_TRUE(answers().empty());
}

/**
 * Given a datagram whose protocol id is not RTYP
 * When the acceptor receives it
 * Then it gets no answer
 */
TEST_F(ConnectionAcceptorTest, IgnoresADatagramOfAnotherProtocol) {
  network::PacketHeader header;
  header.protocolId = OTHER_PROTOCOL_ID;

  receive(playerEndpoint(0), requestDatagram(0, header));

  EXPECT_TRUE(answers().empty());
}

}  // namespace
}  // namespace rtype::server
