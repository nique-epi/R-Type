#include <gtest/gtest.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "ByteWriter.hpp"
#include "ConnectedPlayer.hpp"
#include "ConnectionAccepted.hpp"
#include "ConnectionAcceptedFixtures.hpp"
#include "ConnectionAcceptedTestConstants.hpp"
#include "ConnectionAcceptor.hpp"
#include "ConnectionAcceptorFixture.hpp"
#include "ConnectionAcceptorFixtures.hpp"
#include "ConnectionAcceptorParts.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "EntitySide.hpp"
#include "EntityTypeEntry.hpp"
#include "IncomingDatagram.hpp"
#include "MessageType.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "ProtocolTestConstants.hpp"
#include "ReadyState.hpp"
#include "SentDatagrams.hpp"
#include "ServerDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

using network::testing::acceptanceWithEnemyTypes;
using network::testing::LONGEST_TYPE_NAME;
using network::testing::MOST_TYPES_ON_THE_PROTOCOL_PAGE;
using testing::A_FIFTH_OF_A_SECOND_LATER;
using testing::A_TENTH_OF_A_SECOND_LATER;
using testing::ACCEPTANCE_REPEAT_TIME;
using testing::acceptancesAmong;
using testing::ConnectionAcceptorFixture;
using testing::COOKIE_EXPIRED_TIME;
using testing::DROPPED_DATAGRAM_LINE_TEXT;
using testing::DROPPED_TOGETHER_COUNT;
using testing::emptyNicknameRequestDatagram;
using testing::JUST_AFTER_THE_REPEAT;
using testing::JUST_BEFORE_THE_REPEAT;
using testing::NEXT_ACCEPTANCE_REPEAT_TIME;
using testing::NEXT_PROTOCOL_VERSION;
using testing::nicknameOf;
using testing::onlyMessageIn;
using testing::onlyMessageTypeOf;
using testing::OTHER_PROTOCOL_ID;
using testing::PLAYER_NICKNAME;
using testing::playerEndpoint;
using testing::REFUSAL_DATAGRAM_SIZE;
using testing::REQUEST_TIME;
using testing::SHORTEST_NICKNAME;

/**
 * Given an acceptor with nobody connected
 * When a sender goes through the challenge, then repeats its request
 * Then the acceptor reports the player as joined once, not at the repeat
 */
TEST_F(ConnectionAcceptorFixture, ReportsAJoinedPlayerOnlyOnce) {
  const std::uint64_t cookie = challengeCookie(playerEndpoint(0));
  receive(playerEndpoint(0), requestDatagram(cookie));
  receive(playerEndpoint(0), requestDatagram(cookie), ACCEPTANCE_REPEAT_TIME);

  EXPECT_EQ(joinedPlayers(), (std::vector<std::uint8_t>{1}));
}

/**
 * Given an acceptor with nobody connected
 * When a sender requests a connection without a cookie
 * Then no player is reported as joined: a challenge reserves nothing
 */
TEST_F(ConnectionAcceptorFixture, ReportsNobodyForAChallengedSender) {
  receive(playerEndpoint(0), requestDatagram(0));

  EXPECT_TRUE(joinedPlayers().empty());
}

/**
 * Given an acceptor with nobody connected
 * When a sender requests a connection without a cookie
 * Then it gets a challenge with a cookie, sent back to its address
 */
TEST_F(ConnectionAcceptorFixture, ChallengesARequestWithoutCookie) {
  receive(playerEndpoint(0), requestDatagram(0));

  ASSERT_EQ(answers().size(), 1);
  EXPECT_EQ(lastAnswer().destination, playerEndpoint(0));
  EXPECT_NE(onlyMessageIn<network::ConnectionChallenge>(lastAnswer()).cookie,
            0);
}

/**
 * Given an acceptor with nobody connected
 * When a sender requests a connection without a cookie
 * Then nobody takes a place: a challenge reserves nothing
 */
TEST_F(ConnectionAcceptorFixture, KeepsNothingForAChallengedSender) {
  receive(playerEndpoint(0), requestDatagram(0));

  EXPECT_EQ(roster().find(playerEndpoint(0)), nullptr);
}

/**
 * Given a sender that requested a connection with the smallest request, so a
 * sender that did not prove its address
 * When the acceptor answers it
 * Then the answer holds no more bytes than the request did
 */
TEST_F(ConnectionAcceptorFixture, NeverAnswersAnUnprovenSenderWithMoreBytes) {
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
TEST_F(ConnectionAcceptorFixture, AcceptsARequestCarryingItsCookie) {
  connect(playerEndpoint(0));

  EXPECT_EQ(
      onlyMessageIn<network::ConnectionAccepted>(lastAnswer()).playerNumber,
      network::FIRST_PLAYER_NUMBER);
  EXPECT_NE(
      network::DatagramCodec::decodeHeader(lastAnswer().payload).sessionToken,
      0);
}

/**
 * Given a sender that got a challenge
 * When it sends its request again with the cookie of the challenge
 * Then its address is announced as admitted, once, with the session token its
 * acceptance carries
 */
TEST_F(ConnectionAcceptorFixture, AnnouncesTheAddressAndTokenOfANewPlayer) {
  connect(playerEndpoint(0));

  const std::uint64_t acceptedToken =
      network::DatagramCodec::decodeHeader(lastAnswer().payload).sessionToken;
  EXPECT_EQ(admitted(),
            (std::vector<std::pair<network::Endpoint, std::uint64_t>>{
                {playerEndpoint(0), acceptedToken}}));
}

/**
 * Given an acceptor with nobody connected
 * When a sender is challenged
 * Then nobody is announced as admitted
 */
TEST_F(ConnectionAcceptorFixture, AnnouncesNobodyForAChallenge) {
  receive(playerEndpoint(0), requestDatagram(0));

  EXPECT_TRUE(admitted().empty());
}

/**
 * Given a connected player
 * When it is accepted again
 * Then it is not announced a second time
 */
TEST_F(ConnectionAcceptorFixture, AnnouncesAPlayerAcceptedAgainOnlyOnce) {
  connect(playerEndpoint(0));

  connect(playerEndpoint(0), std::string{PLAYER_NICKNAME},
          ACCEPTANCE_REPEAT_TIME);

  EXPECT_EQ(admitted().size(), 1);
}

/**
 * Given an acceptor with nobody connected
 * When four senders connect one after the other
 * Then they are accepted as players 1, 2, 3 and 4
 */
TEST_F(ConnectionAcceptorFixture, NumbersFourPlayersInTheOrderTheyConnect) {
  for (std::size_t player = 0; player < network::MAXIMUM_PLAYERS; ++player) {
    connect(playerEndpoint(player), nicknameOf(player));

    EXPECT_EQ(
        onlyMessageIn<network::ConnectionAccepted>(lastAnswer()).playerNumber,
        network::FIRST_PLAYER_NUMBER + player);
  }
}

/**
 * Given four connected players
 * When a fifth sender requests a connection, with no cookie yet
 * Then it is refused at once because the game is full
 */
TEST_F(ConnectionAcceptorFixture, RefusesTheFifthPlayerBecauseTheGameIsFull) {
  for (std::size_t player = 0; player < network::MAXIMUM_PLAYERS; ++player) {
    connect(playerEndpoint(player), nicknameOf(player));
  }

  receive(playerEndpoint(network::MAXIMUM_PLAYERS),
          requestDatagramOf(nicknameOf(network::MAXIMUM_PLAYERS), 0));

  EXPECT_EQ(onlyMessageIn<network::ConnectionRefused>(lastAnswer()).reason,
            network::RefusalReason::GameFull);
}

/**
 * Given a connected player named Ace
 * When another sender requests a connection as ACE, with no cookie yet
 * Then it is refused at once because the nickname is taken, and nothing is
 * kept for it
 */
TEST_F(ConnectionAcceptorFixture,
       RefusesANicknameTakenWhateverTheCaseOfItsLetters) {
  connect(playerEndpoint(0), "Ace");

  receive(playerEndpoint(1), requestDatagramOf("ACE", 0));

  EXPECT_EQ(onlyMessageIn<network::ConnectionRefused>(lastAnswer()).reason,
            network::RefusalReason::NicknameTaken);
  EXPECT_EQ(roster().find(playerEndpoint(1)), nullptr);
}

/**
 * Given a player named Ace whose seat is held, away
 * When another sender asks for Ace
 * Then it is refused because the nickname is taken
 */
TEST_F(ConnectionAcceptorFixture, RefusesTheNicknameOfAPlayerWhoseSeatIsHeld) {
  connect(playerEndpoint(0), "Ace");
  roster().find(playerEndpoint(0))->away = true;

  receive(playerEndpoint(1), requestDatagramOf("Ace", 0));

  EXPECT_EQ(onlyMessageIn<network::ConnectionRefused>(lastAnswer()).reason,
            network::RefusalReason::NicknameTaken);
}

/**
 * Given a connected player named Ace
 * When the same sender sends its request again with a valid cookie
 * Then it gets its ConnectionAccepted again, not a refusal as a duplicate of
 * itself
 */
TEST_F(ConnectionAcceptorFixture,
       AcceptsAgainThePlayerWhoSendsItsRequestTwice) {
  connect(playerEndpoint(0), "Ace");

  connect(playerEndpoint(0), "Ace", ACCEPTANCE_REPEAT_TIME);

  EXPECT_EQ(onlyMessageTypeOf(lastAnswer()), network::ConnectionAccepted::TYPE);
}

/**
 * Given a connected player named Ace, who then left
 * When another sender asks for Ace
 * Then it is accepted: the nickname is free again
 */
TEST_F(ConnectionAcceptorFixture, GivesANicknameBackOnceItsPlayerLeft) {
  connect(playerEndpoint(0), "Ace");
  static_cast<void>(roster().remove(playerEndpoint(0)));

  connect(playerEndpoint(1), "Ace");

  EXPECT_EQ(onlyMessageTypeOf(lastAnswer()), network::ConnectionAccepted::TYPE);
}

/**
 * Given a game in progress
 * When a sender that is not connected requests a connection
 * Then it is refused because the game is in progress
 */
TEST_F(ConnectionAcceptorFixture, RefusesANewPlayerWhileAGameIsInProgress) {
  acceptor().setGameInProgress(true);

  receive(playerEndpoint(0), requestDatagram(0));

  EXPECT_EQ(onlyMessageIn<network::ConnectionRefused>(lastAnswer()).reason,
            network::RefusalReason::GameInProgress);
}

/**
 * Given a game that was in progress and is no longer
 * When a sender requests a connection without a cookie
 * Then it is challenged, not refused
 */
TEST_F(ConnectionAcceptorFixture, LetsPlayersInAgainOnceTheGameIsOver) {
  acceptor().setGameInProgress(true);
  acceptor().setGameInProgress(false);

  receive(playerEndpoint(0), requestDatagram(0));

  EXPECT_EQ(onlyMessageTypeOf(lastAnswer()),
            network::MessageType::ConnectionChallenge);
}

/**
 * Given a connected player whose acceptance was lost
 * When it sends its request with a valid cookie again
 * Then it gets the same number and the same session token again
 */
TEST_F(ConnectionAcceptorFixture, AcceptsAConnectedPlayerAgainTheSameWay) {
  connect(playerEndpoint(0));
  const network::OutgoingDatagram first = lastAnswer();

  connect(playerEndpoint(0), std::string{PLAYER_NICKNAME},
          ACCEPTANCE_REPEAT_TIME);

  EXPECT_EQ(onlyMessageIn<network::ConnectionAccepted>(lastAnswer()),
            onlyMessageIn<network::ConnectionAccepted>(first));
  EXPECT_EQ(
      network::DatagramCodec::decodeHeader(lastAnswer().payload).sessionToken,
      network::DatagramCodec::decodeHeader(first.payload).sessionToken);
}

/**
 * Given a connected player accepted once
 * When it is accepted a second time
 * Then the second acceptance carries the next sequence of the session
 */
TEST_F(ConnectionAcceptorFixture, NumbersEachAcceptanceWithTheNextSequence) {
  connect(playerEndpoint(0));
  const std::uint16_t firstSequence =
      network::DatagramCodec::decodeHeader(lastAnswer().payload).sequence;

  connect(playerEndpoint(0), std::string{PLAYER_NICKNAME},
          ACCEPTANCE_REPEAT_TIME);

  EXPECT_EQ(firstSequence, 0);
  EXPECT_EQ(network::DatagramCodec::decodeHeader(lastAnswer().payload).sequence,
            1);
}

/**
 * Given a connected player and a game that started since
 * When the player sends its request with a valid cookie again
 * Then it is accepted again, not refused: it already plays
 */
TEST_F(ConnectionAcceptorFixture, AcceptsAConnectedPlayerDuringItsGame) {
  connect(playerEndpoint(0));
  acceptor().setGameInProgress(true);

  connect(playerEndpoint(0), std::string{PLAYER_NICKNAME},
          ACCEPTANCE_REPEAT_TIME);

  EXPECT_EQ(
      onlyMessageIn<network::ConnectionAccepted>(lastAnswer()).playerNumber,
      network::FIRST_PLAYER_NUMBER);
}

/**
 * Given a connected player and a game that started since
 * When the player sends a request without a cookie
 * Then it gets a challenge, not a refusal: it already has its place
 */
TEST_F(ConnectionAcceptorFixture,
       ChallengesAConnectedPlayerWithoutCookieDuringItsGame) {
  connect(playerEndpoint(0));
  acceptor().setGameInProgress(true);

  receive(playerEndpoint(0), requestDatagram(0));

  EXPECT_EQ(onlyMessageTypeOf(lastAnswer()),
            network::MessageType::ConnectionChallenge);
}

/**
 * Given a sender that got a challenge
 * When it sends a request with another cookie
 * Then it gets a new challenge and takes no place
 */
TEST_F(ConnectionAcceptorFixture, ChallengesARequestWithAWrongCookie) {
  const std::uint64_t cookie = challengeCookie(playerEndpoint(0));

  receive(playerEndpoint(0), requestDatagram(cookie + 1));

  EXPECT_EQ(onlyMessageTypeOf(lastAnswer()),
            network::MessageType::ConnectionChallenge);
  EXPECT_EQ(roster().find(playerEndpoint(0)), nullptr);
}

/**
 * Given a sender that got a challenge
 * When it sends the cookie back two cookie windows later
 * Then the cookie has expired: it gets a new challenge
 */
TEST_F(ConnectionAcceptorFixture, ChallengesARequestWithAnExpiredCookie) {
  const std::uint64_t cookie = challengeCookie(playerEndpoint(0));

  receive(playerEndpoint(0), requestDatagram(cookie), COOKIE_EXPIRED_TIME);

  EXPECT_EQ(onlyMessageTypeOf(lastAnswer()),
            network::MessageType::ConnectionChallenge);
}

/**
 * Given a cookie challenged to one sender
 * When another sender, as one forging an address would, sends it back
 * Then the other sender takes no place
 */
TEST_F(ConnectionAcceptorFixture,
       RefusesThePlaceToASenderWithAnotherOnesCookie) {
  const std::uint64_t cookie = challengeCookie(playerEndpoint(0));

  receive(playerEndpoint(1), requestDatagram(cookie));

  EXPECT_EQ(roster().find(playerEndpoint(1)), nullptr);
}

/**
 * Given an acceptor with nobody connected
 * When an unknown sender sends a message other than a connection request
 * Then it gets no answer and takes no place
 */
TEST_F(ConnectionAcceptorFixture,
       IgnoresAnythingButARequestFromAnUnknownSender) {
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
TEST_F(ConnectionAcceptorFixture, AnswersADatagramOnceWhateverItHolds) {
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
TEST_F(ConnectionAcceptorFixture, IgnoresATruncatedDatagram) {
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
TEST_F(ConnectionAcceptorFixture, IgnoresARequestWithAnInvalidNickname) {
  EXPECT_NO_THROW(receive(playerEndpoint(0), emptyNicknameRequestDatagram()));
  EXPECT_TRUE(answers().empty());
}

/**
 * Given a sender that got the challenge, and its request carrying the cookie
 * of that challenge and a nickname holding a line feed
 * When the acceptor receives the request
 * Then it gets no other answer and takes no place: written in the log of the
 * server, that nickname would start a line of its own
 */
TEST_F(ConnectionAcceptorFixture, IgnoresARequestWhoseNicknameHoldsALineFeed) {
  network::ByteWriter payload;
  payload.writeUint64(challengeCookie(playerEndpoint(0)));
  payload.writeString(network::testing::NICKNAME_WITH_LINE_FEED);

  receive(playerEndpoint(0),
          network::DatagramCodec::encode(
              network::PacketHeader{}, network::MessageType::ConnectionRequest,
              payload.bytes()));

  EXPECT_EQ(answers().size(), 1U);
  EXPECT_EQ(roster().find(playerEndpoint(0)), nullptr);
}

/**
 * Given a request of the next protocol version
 * When the acceptor receives it
 * Then it is refused as IncompatibleVersion, in a header of the version of
 * this server, and nobody takes a place
 */
TEST_F(ConnectionAcceptorFixture, RefusesARequestOfAnotherVersion) {
  network::PacketHeader header;
  header.version = NEXT_PROTOCOL_VERSION;

  receive(playerEndpoint(0), requestDatagram(0, header));

  ASSERT_EQ(answers().size(), 1U);
  EXPECT_EQ(lastAnswer().destination, playerEndpoint(0));
  EXPECT_EQ(network::DatagramCodec::decodeHeader(lastAnswer().payload).version,
            network::PROTOCOL_VERSION);
  EXPECT_EQ(onlyMessageIn<network::ConnectionRefused>(lastAnswer()).reason,
            network::RefusalReason::IncompatibleVersion);
  EXPECT_EQ(roster().find(playerEndpoint(0)), nullptr);
}

/**
 * Given a datagram of another version of 26 bytes, the size of the refusal,
 * that holds no request
 * When the acceptor receives it
 * Then it is refused, with no more bytes than it holds
 */
TEST_F(ConnectionAcceptorFixture,
       RefusesADatagramOfAnotherVersionAsLongAsTheRefusal) {
  network::PacketHeader header;
  header.version = NEXT_PROTOCOL_VERSION;
  const std::vector<std::byte> datagram = readyDatagram(header);
  ASSERT_EQ(datagram.size(), REFUSAL_DATAGRAM_SIZE);

  receive(playerEndpoint(0), datagram);

  ASSERT_EQ(answers().size(), 1U);
  EXPECT_LE(lastAnswer().payload.size(), datagram.size());
  EXPECT_EQ(onlyMessageIn<network::ConnectionRefused>(lastAnswer()).reason,
            network::RefusalReason::IncompatibleVersion);
}

/**
 * Given a datagram of another version of 25 bytes, one fewer than the refusal
 * When the acceptor receives it
 * Then it gets no answer
 */
TEST_F(ConnectionAcceptorFixture,
       IgnoresADatagramOfAnotherVersionShorterThanTheRefusal) {
  network::PacketHeader header;
  header.version = NEXT_PROTOCOL_VERSION;
  const std::vector<std::byte> datagram = disconnectionDatagram(header);
  ASSERT_EQ(datagram.size(), REFUSAL_DATAGRAM_SIZE - 1);

  receive(playerEndpoint(0), datagram);

  EXPECT_TRUE(answers().empty());
}

/**
 * Given a datagram whose protocol id is not RTYP
 * When the acceptor receives it
 * Then it gets no answer
 */
TEST_F(ConnectionAcceptorFixture, IgnoresADatagramOfAnotherProtocol) {
  network::PacketHeader header;
  header.protocolId = OTHER_PROTOCOL_ID;

  receive(playerEndpoint(0), requestDatagram(0, header));

  EXPECT_TRUE(answers().empty());
}

/**
 * Given a request whose nickname is empty
 * When the acceptor receives it
 * Then the journal holds a line about the datagram dropped that names the
 * address and the port of its sender
 */
TEST_F(ConnectionAcceptorFixture, LogsARefusedRequestWithItsSender) {
  receive(playerEndpoint(0), emptyNicknameRequestDatagram());

  const std::vector<std::string> lines =
      journalLinesHolding(DROPPED_DATAGRAM_LINE_TEXT);
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_NE(lines.front().find(senderText(playerEndpoint(0))),
            std::string::npos);
}

/**
 * Given a request without a cookie
 * When the acceptor receives it and challenges its sender
 * Then the journal holds no line about a datagram dropped
 */
TEST_F(ConnectionAcceptorFixture, LogsNothingForARequestItAnswers) {
  receive(playerEndpoint(0), requestDatagram(0));

  ASSERT_EQ(answers().size(), 1U);
  EXPECT_TRUE(journalLinesHolding(DROPPED_DATAGRAM_LINE_TEXT).empty());
}

/**
 * Given DROPPED_TOGETHER_COUNT requests whose nickname is empty
 * When the acceptor receives them at the same time
 * Then the journal holds one line about them, not one per datagram: whoever
 * floods the server cannot flood the log
 */
TEST_F(ConnectionAcceptorFixture, WritesOneLineForRequestsDroppedTogether) {
  for (std::size_t sent = 0; sent < DROPPED_TOGETHER_COUNT; ++sent) {
    receive(playerEndpoint(0), emptyNicknameRequestDatagram());
  }

  EXPECT_EQ(journalLinesHolding(DROPPED_DATAGRAM_LINE_TEXT).size(), 1U);
}

/**
 * Given an acceptor that logged a request it refused
 * When another one arrives REJECTION_LINE_INTERVAL later
 * Then the journal holds a second line: the interval is measured on the time
 * the acceptor is given
 */
TEST_F(ConnectionAcceptorFixture, WritesTheNextLineOnceTheIntervalIsOver) {
  receive(playerEndpoint(0), emptyNicknameRequestDatagram());

  receive(playerEndpoint(0), emptyNicknameRequestDatagram(),
          REQUEST_TIME + network::REJECTION_LINE_INTERVAL);

  EXPECT_EQ(journalLinesHolding(DROPPED_DATAGRAM_LINE_TEXT).size(), 2U);
}

/**
 * Given an acceptor whose report of a joined player throws, and a sender
 * that went through the challenge
 * When that sender's request with its cookie and a request without a cookie
 * from a second sender, under another nickname, arrive in the same tick
 * Then the second sender is still challenged: only the datagram whose answer
 * threw is dropped
 */
TEST(ConnectionAcceptor, AnswersTheNextRequestAfterOneWhoseAnswerThrew) {
  PlayerRoster roster;
  std::vector<network::OutgoingDatagram> answers;
  ConnectionAcceptor acceptor{ConnectionAcceptorParts{
      .roster = roster,
      .send =
          [&answers](network::OutgoingDatagram datagram) {
            answers.push_back(std::move(datagram));
          },
      .admit = [](const network::Endpoint&, std::uint64_t) {},
      .onPlayerJoined =
          [](ConnectedPlayer&) { throw network::InvalidCoordinateException(); },
      .entityTypes = acceptanceWithEnemyTypes(MOST_TYPES_ON_THE_PROTOCOL_PAGE,
                                              LONGEST_TYPE_NAME)
                         .types}};
  const network::IncomingDatagram firstRequest{.sender = playerEndpoint(0),
                                               .payload = requestDatagram(0)};
  acceptor.receive(std::span{&firstRequest, 1}, REQUEST_TIME);
  const std::uint64_t cookie =
      onlyMessageIn<network::ConnectionChallenge>(answers.back()).cookie;
  const std::array datagrams{
      network::IncomingDatagram{.sender = playerEndpoint(0),
                                .payload = requestDatagram(cookie)},
      network::IncomingDatagram{
          .sender = playerEndpoint(1),
          .payload = requestDatagramOf(nicknameOf(1), 0)}};

  acceptor.receive(datagrams, REQUEST_TIME);

  EXPECT_EQ(answers.back().destination, playerEndpoint(1));
  EXPECT_EQ(onlyMessageTypeOf(answers.back()),
            network::MessageType::ConnectionChallenge);
}

/**
 * Given a sender that got a challenge, on a server of the sheets the game
 * ships with
 * When it sends its request again with the cookie of the challenge
 * Then its acceptance lists every sheet in the order of its name, which is the
 * order of its number, with its side
 */
TEST_F(ConnectionAcceptorFixture, AcceptsWithTheEntityTypesOfTheGame) {
  connect(playerEndpoint(0));

  EXPECT_EQ(
      onlyMessageIn<network::ConnectionAccepted>(lastAnswer()).types,
      (std::vector<network::EntityTypeEntry>{
          {.side = network::EntitySide::Enemy, .name = "bydo_tyrant"},
          {.side = network::EntitySide::Enemy, .name = "bydomancer"},
          {.side = network::EntitySide::Enemy, .name = "drifter"},
          {.side = network::EntitySide::Enemy, .name = "drillback"},
          {.side = network::EntitySide::EnemyMissile, .name = "enemy_missile"},
          {.side = network::EntitySide::EnemyMissile, .name = "orb_shard"},
          {.side = network::EntitySide::PlayerMissile,
           .name = "player_missile"},
          {.side = network::EntitySide::Enemy, .name = "reaper"},
          {.side = network::EntitySide::EnemyMissile, .name = "rune_orb"},
          {.side = network::EntitySide::Player, .name = "ship"},
          {.side = network::EntitySide::Enemy, .name = "tyrant_diver"},
          {.side = network::EntitySide::EnemyMissile, .name = "tyrant_missile"},
          {.side = network::EntitySide::EnemyMissile, .name = "tyrant_spore"},
          {.side = network::EntitySide::EnemyMissile,
           .name = "tyrant_spore_shard"},
          {.side = network::EntitySide::Enemy, .name = "voltjelly"},
          {.side = network::EntitySide::Enemy, .name = "weaver"}}));
}

/**
 * Given a connected player accepted at the time of its first request
 * When it sends its request with a valid cookie again 100, 200 and 249 ms
 * later
 * Then none of them is answered: the acceptance goes out at most once per
 * 250 ms to a player
 */
TEST_F(ConnectionAcceptorFixture, HoldsBackTheAcceptanceWithinTheInterval) {
  const std::uint64_t cookie = challengeCookie(playerEndpoint(0));
  receive(playerEndpoint(0), requestDatagram(cookie));

  for (const engine::Duration later :
       {A_TENTH_OF_A_SECOND_LATER, A_FIFTH_OF_A_SECOND_LATER,
        JUST_BEFORE_THE_REPEAT}) {
    receive(playerEndpoint(0), requestDatagram(cookie), later);
  }

  EXPECT_EQ(acceptancesAmong(answers()), 1U);
}

/**
 * Given a connected player accepted at the time of its first request
 * When it sends its request with a valid cookie again 250 ms later
 * Then it gets its acceptance again
 */
TEST_F(ConnectionAcceptorFixture, RepeatsTheAcceptanceOnceTheIntervalPassed) {
  const std::uint64_t cookie = challengeCookie(playerEndpoint(0));
  receive(playerEndpoint(0), requestDatagram(cookie));

  receive(playerEndpoint(0), requestDatagram(cookie), ACCEPTANCE_REPEAT_TIME);

  EXPECT_EQ(acceptancesAmong(answers()), 2U);
}

/**
 * Given a connected player accepted again 250 ms after its first acceptance
 * When it sends its request again 300 ms, then 500 ms after the first
 * Then the first is held back and the second answered: the interval counts
 * from the last acceptance, not from the first
 */
TEST_F(ConnectionAcceptorFixture, CountsTheIntervalFromTheLastAcceptance) {
  const std::uint64_t cookie = challengeCookie(playerEndpoint(0));
  receive(playerEndpoint(0), requestDatagram(cookie));
  receive(playerEndpoint(0), requestDatagram(cookie), ACCEPTANCE_REPEAT_TIME);

  receive(playerEndpoint(0), requestDatagram(cookie), JUST_AFTER_THE_REPEAT);
  EXPECT_EQ(acceptancesAmong(answers()), 2U);

  receive(playerEndpoint(0), requestDatagram(cookie),
          NEXT_ACCEPTANCE_REPEAT_TIME);
  EXPECT_EQ(acceptancesAmong(answers()), 3U);
}

/**
 * Given a connected player accepted again 250 ms later
 * When its two acceptances are compared
 * Then both carry the same table of entity types
 */
TEST_F(ConnectionAcceptorFixture, RepeatsTheSameTableOfEntityTypes) {
  connect(playerEndpoint(0));
  const network::OutgoingDatagram first = lastAnswer();

  connect(playerEndpoint(0), std::string{PLAYER_NICKNAME},
          ACCEPTANCE_REPEAT_TIME);

  EXPECT_EQ(onlyMessageIn<network::ConnectionAccepted>(lastAnswer()).types,
            onlyMessageIn<network::ConnectionAccepted>(first).types);
}

/**
 * Given a player accepted just now, who then left
 * When a new session asks from the same address 100 ms later, with a valid
 * cookie
 * Then it is accepted at once: the pace belonged to the player who left
 */
TEST_F(ConnectionAcceptorFixture, GivesANewSessionOnTheSameAddressNoWait) {
  connect(playerEndpoint(0));
  static_cast<void>(roster().remove(playerEndpoint(0)));

  connect(playerEndpoint(0), std::string{PLAYER_NICKNAME},
          A_TENTH_OF_A_SECOND_LATER);

  EXPECT_EQ(acceptancesAmong(answers()), 2U);
}

/**
 * Given an acceptor whose game holds the largest table of entity types
 * When a sender sends the smallest request, without a cookie
 * Then its challenge holds no more bytes than the request: the table never
 * reaches an address that did not prove itself
 */
TEST(ConnectionAcceptor, ChallengesWithNoMoreBytesWhateverTheTable) {
  PlayerRoster roster;
  std::vector<network::OutgoingDatagram> answers;
  ConnectionAcceptor acceptor{ConnectionAcceptorParts{
      .roster = roster,
      .send =
          [&answers](network::OutgoingDatagram datagram) {
            answers.push_back(std::move(datagram));
          },
      .admit = [](const network::Endpoint&, std::uint64_t) {},
      .onPlayerJoined = {},
      .entityTypes = acceptanceWithEnemyTypes(MOST_TYPES_ON_THE_PROTOCOL_PAGE,
                                              LONGEST_TYPE_NAME)
                         .types}};
  const network::IncomingDatagram request{
      .sender = playerEndpoint(0),
      .payload = network::DatagramCodec::encode(
          network::PacketHeader{},
          network::ConnectionRequest{
              .cookie = 0, .nickname = std::string{SHORTEST_NICKNAME}})};

  acceptor.receive(std::span{&request, 1}, REQUEST_TIME);

  ASSERT_EQ(answers.size(), 1U);
  EXPECT_LE(answers.front().payload.size(), request.payload.size());
}

}  // namespace rtype::server
