#include <gmock/gmock.h>
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
#include "DebugJournal.hpp"
#include "Disconnection.hpp"
#include "EntityAnnouncements.hpp"
#include "EntityReplicatorTestConstants.hpp"
#include "GameStarted.hpp"
#include "Heartbeat.hpp"
#include "NetworkTestConstants.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "ServerSessionFixture.hpp"
#include "ServerSessionFixtures.hpp"
#include "ServerSessionLogFixture.hpp"
#include "ServerSessionTestConstants.hpp"
#include "TimeConstants.hpp"
#include "TimeTestConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateFilterTestConstants.hpp"

namespace rtype::client {

using testing::ACCEPTANCE_SEQUENCE;
using testing::ACCEPTED_PLAYER_NUMBER;
using testing::APPLIED_TICK;
using testing::CHALLENGE_COOKIE;
using testing::FIRST_POSITION;
using testing::FIRST_SEQUENCE;
using testing::holds;
using testing::listed;
using testing::LOST_SERVER_LINE_TEXT;
using testing::NEXT_PROTOCOL_VERSION;
using testing::NICKNAME;
using testing::OTHER_SESSION_TOKEN;
using testing::PROTOCOL_ATTEMPT_TIMEOUT;
using testing::PROTOCOL_DISCONNECTION_COPY_COUNT;
using testing::PROTOCOL_DISCONNECTION_COPY_INTERVAL;
using testing::PROTOCOL_FIRST_INPUT_NUMBER;
using testing::PROTOCOL_HEARTBEAT_INTERVAL;
using testing::PROTOCOL_READY_STATE_INTERVAL;
using testing::PROTOCOL_RECONNECTION_GRACE;
using testing::PROTOCOL_REQUEST_INTERVAL;
using testing::PROTOCOL_SESSION_TIMEOUT;
using testing::PROTOCOL_SILENCE_LIMIT;
using testing::PROTOCOL_UP_ACTION_BIT;
using testing::server;
using testing::SERVER_VERSION;
using testing::ServerSessionFixture;
using testing::ServerSessionLogFixture;
using testing::SESSION_TOKEN;
using testing::SHIP_ID;
using ::testing::SizeIs;
using testing::stateListing;
using testing::upHeld;

using Phase = ConnectionStatus::Phase;

/**
 * Given the delays of the protocol page
 * When the connection's request interval and timeout are read
 * Then they are 500 ms and 5 s
 */
TEST(ServerSessionDelays, AreThoseOfTheProtocolPage) {
  EXPECT_EQ(CONNECTION_REQUEST_INTERVAL, PROTOCOL_REQUEST_INTERVAL);
  EXPECT_EQ(CONNECTION_ATTEMPT_TIMEOUT, PROTOCOL_ATTEMPT_TIMEOUT);
}

/**
 * Given a session with no attempt
 * When it connects to a server
 * Then its link listens to that server's datagrams and no other's
 */
TEST_F(ServerSessionFixture, ListensToTheServerItConnectsTo) {
  connect();

  EXPECT_EQ(listenedTo(), server());
}

/**
 * Given a connection that never started an attempt
 * When its status is read
 * Then there is no attempt
 */
TEST_F(ServerSessionFixture, StartsWithNoAttempt) {
  EXPECT_EQ(phase(), Phase::Idle);
}

/**
 * Given a connection with no attempt
 * When it connects to a server
 * Then a request without a cookie, carrying the nickname, goes to that server
 * at once
 */
TEST_F(ServerSessionFixture, SendsARequestWithoutCookieAtOnce) {
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
TEST_F(ServerSessionFixture, WaitsOneIntervalBeforeSendingAgain) {
  connect();

  update(CONNECTION_REQUEST_INTERVAL - ONE_NANOSECOND);

  EXPECT_EQ(sent().size(), 1);
}

/**
 * Given an attempt that just started
 * When one request interval passes without an answer
 * Then the request is sent again, since UDP may have lost it
 */
TEST_F(ServerSessionFixture, SendsTheRequestAgainAfterOneInterval) {
  connect();

  update(CONNECTION_REQUEST_INTERVAL);

  EXPECT_EQ(sent().size(), 2);
}

/**
 * Given an attempt that sent its request twice
 * When the headers of the two requests are read
 * Then they carry the sequences 0 and 1
 */
TEST_F(ServerSessionFixture, NumbersEachRequestWithTheNextSequence) {
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
TEST_F(ServerSessionFixture, AnswersAChallengeAtOnceWithItsCookie) {
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
TEST_F(ServerSessionFixture, KeepsTheCookieForTheRequestsThatFollow) {
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
TEST_F(ServerSessionFixture, IsAcceptedWithThePlayerNumberOfTheServer) {
  connect();
  network::PacketHeader header;
  header.sessionToken = SESSION_TOKEN;
  deliver(network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER},
          server(), header);

  update();

  EXPECT_EQ(session().status(),
            (ConnectionStatus{.phase = Phase::Accepted,
                              .playerNumber = ACCEPTED_PLAYER_NUMBER}));
}

/**
 * Given an attempt in progress
 * When the server refuses it because the game is full
 * Then the status says refused because the game is full
 */
TEST_F(ServerSessionFixture, IsRefusedWithTheReasonOfTheServer) {
  connect();
  deliver(
      network::ConnectionRefused{.reason = network::RefusalReason::GameFull});

  update();

  EXPECT_EQ(session().status(),
            (ConnectionStatus{.phase = Phase::Refused,
                              .refusal = network::RefusalReason::GameFull,
                              .serverVersion = SERVER_VERSION}));
}

/**
 * Given an attempt the server accepted
 * When two more intervals pass
 * Then no request is sent again
 */
TEST_F(ServerSessionFixture, SendsNoRequestOnceAccepted) {
  connect();
  deliver(network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER});
  update();

  update(2 * CONNECTION_REQUEST_INTERVAL);

  EXPECT_EQ(sentCountOf<network::ConnectionRequest>(), 1U);
}

/**
 * Given an attempt with no answer
 * When the attempt timeout minus one nanosecond has passed
 * Then it is still connecting
 */
TEST_F(ServerSessionFixture, KeepsTryingUntilTheTimeout) {
  connect();

  update(CONNECTION_ATTEMPT_TIMEOUT - ONE_NANOSECOND);

  EXPECT_EQ(phase(), Phase::Connecting);
}

/**
 * Given an attempt with no answer
 * When the attempt timeout has passed
 * Then the server is unreachable
 */
TEST_F(ServerSessionFixture, GivesUpAtTheTimeout) {
  connect();

  update(CONNECTION_ATTEMPT_TIMEOUT);

  EXPECT_EQ(phase(), Phase::Unreachable);
}

/**
 * Given an attempt that gave up
 * When another interval passes
 * Then no request is sent again
 */
TEST_F(ServerSessionFixture, SendsNothingOnceItGaveUp) {
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
TEST_F(ServerSessionFixture, ChallengeDoesNotRestartTheTimeout) {
  connect();
  update(CONNECTION_ATTEMPT_TIMEOUT - ONE_NANOSECOND);
  deliver(network::ConnectionChallenge{.cookie = CHALLENGE_COOKIE});
  update();

  update(ONE_NANOSECOND);

  EXPECT_EQ(phase(), Phase::Unreachable);
}

/**
 * Given an attempt in progress
 * When the server's acceptance arrives with its last byte cut off
 * Then the attempt is still connecting, and nothing is thrown
 */
TEST_F(ServerSessionFixture, IgnoresATruncatedDatagram) {
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
 * When the server answers in a datagram of another protocol version, whatever
 * it holds
 * Then the attempt is refused as IncompatibleVersion, with the version the
 * server wrote, and nothing in the datagram is read
 */
TEST_F(ServerSessionFixture, IsRefusedByAnAnswerOfAnotherVersion) {
  connect();
  network::PacketHeader header;
  header.version = NEXT_PROTOCOL_VERSION;
  deliver(network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER},
          server(), header);

  update();

  EXPECT_EQ(
      session().status(),
      (ConnectionStatus{.phase = Phase::Refused,
                        .refusal = network::RefusalReason::IncompatibleVersion,
                        .serverVersion = NEXT_PROTOCOL_VERSION}));
}

/**
 * Given an attempt the server answered in another protocol version
 * When two more intervals pass
 * Then no request is sent again
 */
TEST_F(ServerSessionFixture, SendsNoRequestOnceRefusedForItsVersion) {
  connect();
  network::PacketHeader header;
  header.version = NEXT_PROTOCOL_VERSION;
  deliver(
      network::ConnectionRefused{
          .reason = network::RefusalReason::IncompatibleVersion},
      server(), header);
  update();

  update(2 * CONNECTION_REQUEST_INTERVAL);

  EXPECT_EQ(sentCountOf<network::ConnectionRequest>(), 1U);
}

/**
 * Given a session the server accepted
 * When a datagram of another protocol version comes from the server
 * Then the session goes on: only an attempt ends this way
 */
TEST_F(ServerSessionFixture, KeepsTheSessionWhenAnotherVersionArrives) {
  accept();
  network::PacketHeader header;
  header.version = NEXT_PROTOCOL_VERSION;
  header.sessionToken = SESSION_TOKEN;
  deliver(network::Heartbeat{}, server(), header);

  update();

  EXPECT_EQ(phase(), Phase::Accepted);
}

/**
 * Given an attempt in progress
 * When it is cancelled and another interval passes
 * Then there is no attempt and no request is sent again
 */
TEST_F(ServerSessionFixture, SendsNothingOnceCancelled) {
  connect();

  session().cancel();
  update(CONNECTION_REQUEST_INTERVAL);

  EXPECT_EQ(phase(), Phase::Idle);
  EXPECT_EQ(sent().size(), 1);
}

/**
 * Given an attempt that was cancelled
 * When the server's acceptance arrives late
 * Then there is still no attempt
 */
TEST_F(ServerSessionFixture, IgnoresALateAnswerOnceCancelled) {
  connect();
  session().cancel();
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
TEST_F(ServerSessionFixture, StartsANewAttemptWithoutStaleAnswers) {
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
TEST(ServerSessionDelays, SessionRepetitionsAreThoseOfTheProtocolPage) {
  EXPECT_EQ(READY_STATE_INTERVAL, PROTOCOL_READY_STATE_INTERVAL);
  EXPECT_EQ(DISCONNECTION_COPY_COUNT, PROTOCOL_DISCONNECTION_COPY_COUNT);
  EXPECT_EQ(DISCONNECTION_COPY_INTERVAL, PROTOCOL_DISCONNECTION_COPY_INTERVAL);
}

/**
 * Given a session
 * When the player leaves it, then time passes, step by step
 * Then a Disconnection goes at once, then every 50 ms, three in all
 */
TEST_F(ServerSessionFixture, LeavingSendsThreeDisconnections50MsApart) {
  accept();

  session().cancel();
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
 * When the player leaves it, then time passes by one copy interval at a time
 * Then the connection is leaving until the third Disconnection went out, and
 * no longer once it did
 */
TEST_F(ServerSessionFixture, IsLeavingUntilTheLastDisconnectionWentOut) {
  accept();

  session().cancel();
  EXPECT_TRUE(session().isLeaving());
  update(DISCONNECTION_COPY_INTERVAL);
  EXPECT_TRUE(session().isLeaving());
  update(DISCONNECTION_COPY_INTERVAL);

  EXPECT_FALSE(session().isLeaving());
  EXPECT_EQ(sentCountOf<network::Disconnection>(), 3U);
}

/**
 * Given an attempt the server has not answered yet
 * When the player cancels it
 * Then the connection is not leaving: no session was open, so no
 * Disconnection is due
 */
TEST_F(ServerSessionFixture, IsNotLeavingAnAttemptNeverAccepted) {
  connect();

  session().cancel();

  EXPECT_FALSE(session().isLeaving());
}

/**
 * Given a session
 * When the player leaves it
 * Then the Disconnection goes to that server, with the token of the session
 */
TEST_F(ServerSessionFixture, TheDisconnectionCarriesTheTokenOfTheSession) {
  accept();

  session().cancel();

  ASSERT_TRUE(holds<network::Disconnection>(sent().back()));
  EXPECT_EQ(sent().back().destination, server());
  EXPECT_EQ(
      network::DatagramCodec::decodeHeader(sent().back().payload).sessionToken,
      SESSION_TOKEN);
}

/**
 * Given a session
 * When the client connects again
 * Then a Disconnection to the session left goes before the new request
 */
TEST_F(ServerSessionFixture, ConnectingAgainLeavesTheSessionFirst) {
  accept();

  connect();

  ASSERT_GE(sent().size(), 2U);
  EXPECT_TRUE(holds<network::Disconnection>(sent().at(sent().size() - 2)));
  EXPECT_TRUE(holds<network::ConnectionRequest>(sent().back()));
}

/**
 * Given an attempt the server has not answered yet
 * When the game sends an input
 * Then nothing is sent but the request
 */
TEST_F(ServerSessionFixture, SendsNoInputBeforeTheServerAccepts) {
  connect();

  session().sendPlayerInput(upHeld());

  EXPECT_EQ(sent().size(), 1);
}

/**
 * Given an attempt the server accepted
 * When the game sends an input with up held
 * Then the server gets a PlayerInput numbered 1 with only the bit of up
 */
TEST_F(ServerSessionFixture, SendsTheFirstInputOnceAccepted) {
  connectAndGetAccepted();

  session().sendPlayerInput(upHeld());

  ASSERT_EQ(sent().size(), 2);
  EXPECT_EQ(sent().back().destination, server());
  EXPECT_EQ(lastInput(),
            (network::PlayerInput{.inputNumber = PROTOCOL_FIRST_INPUT_NUMBER,
                                  .actions = PROTOCOL_UP_ACTION_BIT}));
}

/**
 * Given an attempt the server accepted with a session token
 * When the game sends an input
 * Then the header of the input carries that token
 */
TEST_F(ServerSessionFixture, InputsCarryTheSessionTokenOfTheAcceptance) {
  connectAndGetAccepted();

  session().sendPlayerInput(upHeld());

  EXPECT_EQ(lastHeader().sessionToken, SESSION_TOKEN);
}

/**
 * Given an attempt the server accepted, which sent its first input
 * When the game sends another input
 * Then that input is numbered 2
 */
TEST_F(ServerSessionFixture, NumbersEachInputAfterThePreviousOne) {
  connectAndGetAccepted();
  session().sendPlayerInput(upHeld());

  session().sendPlayerInput(upHeld());

  EXPECT_EQ(lastInput().inputNumber, PROTOCOL_FIRST_INPUT_NUMBER + 1);
}

/**
 * Given an attempt accepted after one request, sequence 0
 * When the game sends an input
 * Then the input carries sequence 1: requests and inputs share one counter
 */
TEST_F(ServerSessionFixture, InputsTakeTheSequenceAfterTheRequests) {
  connectAndGetAccepted();

  session().sendPlayerInput(upHeld());

  EXPECT_EQ(lastHeader().sequence, 1);
}

/**
 * Given an attempt the server accepted with a session token
 * When the client connects again
 * Then the new request carries token 0, as every request does
 */
TEST_F(ServerSessionFixture, ANewAttemptDropsTheTokenOfTheOldSession) {
  connectAndGetAccepted();

  connect();

  EXPECT_EQ(lastHeader().sessionToken, 0);
}

/**
 * Given a session that sent two inputs
 * When the client connects again, is accepted, and the game sends an input
 * Then that input is numbered 1: a new session numbers its inputs from 1
 */
TEST_F(ServerSessionFixture, ANewSessionNumbersItsInputsFromOne) {
  connectAndGetAccepted();
  session().sendPlayerInput(upHeld());
  session().sendPlayerInput(upHeld());
  connectAndGetAccepted();

  session().sendPlayerInput(upHeld());

  EXPECT_EQ(lastInput().inputNumber, PROTOCOL_FIRST_INPUT_NUMBER);
}

/**
 * Given an attempt the server refused
 * When the game sends an input
 * Then nothing is sent but the request
 */
TEST_F(ServerSessionFixture, SendsNoInputOnceRefused) {
  connect();
  deliver(
      network::ConnectionRefused{.reason = network::RefusalReason::GameFull});
  update();

  session().sendPlayerInput(upHeld());

  EXPECT_EQ(sent().size(), 1);
}

/**
 * Given an attempt the server accepted, then cancelled
 * When the game sends an input
 * Then no input is sent: only the request and the Disconnection of the
 * session left went out
 */
TEST_F(ServerSessionFixture, SendsNoInputOnceCancelled) {
  connectAndGetAccepted();
  session().cancel();

  session().sendPlayerInput(upHeld());

  EXPECT_EQ(sentCountOf<network::PlayerInput>(), 0U);
}

/**
 * Given a session the server opened
 * When the heartbeat interval minus one nanosecond passes without sending
 * Then no Heartbeat is sent yet
 */
TEST_F(ServerSessionFixture, WaitsOneIntervalBeforeTheFirstHeartbeat) {
  accept();

  update(PROTOCOL_HEARTBEAT_INTERVAL - ONE_NANOSECOND);

  EXPECT_EQ(sentCountOf<network::Heartbeat>(), 0U);
}

/**
 * Given a session the server opened
 * When the heartbeat interval passes without sending anything
 * Then a Heartbeat goes to the server, so it knows the player is still there
 */
TEST_F(ServerSessionFixture, SendsAHeartbeatAfterAnIntervalWithoutSending) {
  accept();

  update(PROTOCOL_HEARTBEAT_INTERVAL);

  ASSERT_EQ(sentCountOf<network::Heartbeat>(), 1U);
  EXPECT_EQ(sent().back().destination, server());
}

/**
 * Given a session the server opened with its token
 * When a Heartbeat goes out
 * Then it carries that token
 */
TEST_F(ServerSessionFixture, TheHeartbeatCarriesTheTokenOfTheSession) {
  accept();

  update(PROTOCOL_HEARTBEAT_INTERVAL);

  ASSERT_TRUE(holds<network::Heartbeat>(sent().back()));
  EXPECT_EQ(lastHeader().sessionToken, SESSION_TOKEN);
}

/**
 * Given a session accepted after one request, sequence 0
 * When a Heartbeat goes out
 * Then it carries sequence 1: it takes the counter of every other datagram
 */
TEST_F(ServerSessionFixture, TheHeartbeatTakesTheSequenceAfterTheRequest) {
  accept();

  update(PROTOCOL_HEARTBEAT_INTERVAL);

  ASSERT_TRUE(holds<network::Heartbeat>(sent().back()));
  EXPECT_EQ(lastHeader().sequence, 1);
}

/**
 * Given a session in which half an interval passed, then an input went out
 * When the interval minus one nanosecond passes after that input
 * Then no Heartbeat is sent: the delay counts from the last datagram sent
 */
TEST_F(ServerSessionFixture, AnInputSentRestartsTheHeartbeatDelay) {
  accept();
  update(PROTOCOL_HEARTBEAT_INTERVAL / 2);
  session().sendPlayerInput(upHeld());

  update(PROTOCOL_HEARTBEAT_INTERVAL - ONE_NANOSECOND);

  EXPECT_EQ(sentCountOf<network::Heartbeat>(), 0U);
}

/**
 * Given a session that just sent a Heartbeat after an interval of silence
 * When the interval minus one nanosecond passes
 * Then no second Heartbeat is sent: a Heartbeat starts its own delay over
 */
TEST_F(ServerSessionFixture, AHeartbeatStartsItsOwnDelayOver) {
  accept();
  update(PROTOCOL_HEARTBEAT_INTERVAL);
  ASSERT_EQ(sentCountOf<network::Heartbeat>(), 1U);

  update(PROTOCOL_HEARTBEAT_INTERVAL - ONE_NANOSECOND);

  EXPECT_EQ(sentCountOf<network::Heartbeat>(), 1U);
}

/**
 * Given a session in which nothing else is due
 * When three intervals pass, one per update
 * Then a Heartbeat goes out after each of them
 */
TEST_F(ServerSessionFixture, SendsAHeartbeatEveryIntervalWhileSilent) {
  accept();

  update(PROTOCOL_HEARTBEAT_INTERVAL);
  update(PROTOCOL_HEARTBEAT_INTERVAL);
  update(PROTOCOL_HEARTBEAT_INTERVAL);

  EXPECT_EQ(sentCountOf<network::Heartbeat>(), 3U);
}

/**
 * Given an attempt the server refused
 * When the heartbeat interval passes
 * Then no Heartbeat is sent: there is no session
 */
TEST_F(ServerSessionFixture, SendsNoHeartbeatOnceRefused) {
  connect();
  deliver(
      network::ConnectionRefused{.reason = network::RefusalReason::GameFull});
  update();

  update(PROTOCOL_HEARTBEAT_INTERVAL);

  EXPECT_EQ(sentCountOf<network::Heartbeat>(), 0U);
}

/**
 * Given a session the player left
 * When the heartbeat interval passes twice
 * Then no Heartbeat is sent to the session left
 */
TEST_F(ServerSessionFixture, SendsNoHeartbeatOnceCancelled) {
  accept();
  session().cancel();

  update(PROTOCOL_HEARTBEAT_INTERVAL);
  update(PROTOCOL_HEARTBEAT_INTERVAL);

  EXPECT_EQ(sentCountOf<network::Heartbeat>(), 0U);
}

/**
 * Given a session the server opened
 * When a Heartbeat of the server arrives
 * Then the journal holds no line about a message without a handler: a
 * Heartbeat only says that the server is still there
 */
TEST_F(ServerSessionLogFixture, ReadsAHeartbeatOfTheServerWithoutLoggingIt) {
  accept();
  deliverInSession(network::Heartbeat{}, FIRST_SEQUENCE);

  update();

  EXPECT_TRUE(journal().linesHolding(SKIPPED_MESSAGE_LINE_TEXT).empty());
}

/**
 * Given a session the server opened, then sent nothing more
 * When the session timeout of 5 s passes
 * Then the session holds, and a Heartbeat has gone to the server: the server
 * keeps the seat for the reconnection grace
 */
TEST_F(ServerSessionFixture, KeepsTheSessionAndItsHeartbeatsPastTheTimeout) {
  accept();

  update(PROTOCOL_SESSION_TIMEOUT);

  EXPECT_EQ(phase(), Phase::Accepted);
  EXPECT_GE(sentCountOf<network::Heartbeat>(), 1U);
}

/**
 * Given a session silent for the timeout and the grace, less one nanosecond
 * When a Heartbeat of the server in the session is read
 * Then the session holds, and survives a further timeout of silence
 */
TEST_F(ServerSessionFixture, ResumesWhenTheServerAnswersDuringTheGrace) {
  accept();
  update(PROTOCOL_SESSION_TIMEOUT + PROTOCOL_RECONNECTION_GRACE -
         ONE_NANOSECOND);
  deliverInSession(network::Heartbeat{}, FIRST_SEQUENCE);
  update();

  update(PROTOCOL_SESSION_TIMEOUT);

  EXPECT_EQ(phase(), Phase::Accepted);
}

/**
 * Given a session the server opened, then sent nothing more
 * When the silence limit minus one nanosecond passes
 * Then the session holds
 */
TEST_F(ServerSessionFixture, KeepsTheSessionUntilTheServerTimeout) {
  accept();

  update(PROTOCOL_SILENCE_LIMIT - ONE_NANOSECOND);

  EXPECT_EQ(phase(), Phase::Accepted);
}

/**
 * Given a session the server opened, then sent nothing more
 * When the silence limit passes
 * Then the connection is lost
 */
TEST_F(ServerSessionFixture, LosesTheServerAtTheTimeoutWithoutADatagram) {
  accept();

  update(PROTOCOL_SILENCE_LIMIT);

  EXPECT_EQ(phase(), Phase::Lost);
}

/**
 * Given a session silent for the silence limit minus one nanosecond, then a
 * Heartbeat of the server read
 * When the silence limit minus one nanosecond passes again
 * Then the session holds: the Heartbeat started the delay over
 */
TEST_F(ServerSessionFixture, AHeartbeatOfTheServerRestartsTheTimeout) {
  accept();
  update(PROTOCOL_SILENCE_LIMIT - ONE_NANOSECOND);
  deliverInSession(network::Heartbeat{}, FIRST_SEQUENCE);
  update();

  update(PROTOCOL_SILENCE_LIMIT - ONE_NANOSECOND);

  EXPECT_EQ(phase(), Phase::Accepted);
}

/**
 * Given a game silent for the silence limit minus one nanosecond, then a
 * WorldState read, which a server never follows with a Heartbeat while states
 * go out
 * When the silence limit minus one nanosecond passes again
 * Then the session holds: any datagram of the session starts the delay over
 */
TEST_F(ServerSessionFixture, AWorldStateRestartsTheTimeout) {
  accept();
  update(PROTOCOL_SILENCE_LIMIT - ONE_NANOSECOND);
  deliverWorldStateInSession(
      stateListing(APPLIED_TICK,
                   {listed(SHIP_ID, network::WireEntityType::PlayerShip,
                           FIRST_POSITION)}),
      FIRST_SEQUENCE);
  update();

  update(PROTOCOL_SILENCE_LIMIT - ONE_NANOSECOND);

  EXPECT_EQ(phase(), Phase::Accepted);
}

/**
 * Given a session silent for the silence limit minus one nanosecond, then a
 * Heartbeat from the address of the server carrying another token
 * When one more nanosecond passes
 * Then the connection is lost: a forged datagram keeps no session alive
 */
TEST_F(ServerSessionFixture, ADatagramWithAnotherTokenKeepsNoSessionAlive) {
  accept();
  update(PROTOCOL_SILENCE_LIMIT - ONE_NANOSECOND);
  network::PacketHeader forged;
  forged.sessionToken = OTHER_SESSION_TOKEN;
  forged.sequence = FIRST_SEQUENCE;
  deliver(network::Heartbeat{}, server(), forged);
  update();

  update(ONE_NANOSECOND);

  EXPECT_EQ(phase(), Phase::Lost);
}

/**
 * Given a session that read a Heartbeat, then stayed silent for the timeout
 * minus one nanosecond, then got a copy of that Heartbeat
 * When one more nanosecond passes
 * Then the connection is lost: a copy says nothing newer than the original
 */
TEST_F(ServerSessionFixture, ACopyOfADatagramReadKeepsNoSessionAlive) {
  accept();
  deliverInSession(network::Heartbeat{}, FIRST_SEQUENCE);
  update();
  update(PROTOCOL_SILENCE_LIMIT - ONE_NANOSECOND);
  deliverInSession(network::Heartbeat{}, FIRST_SEQUENCE);
  update();

  update(ONE_NANOSECOND);

  EXPECT_EQ(phase(), Phase::Lost);
}

/**
 * Given a session silent for the silence limit minus one nanosecond, then a
 * Heartbeat of the session with its last byte cut off
 * When one more nanosecond passes
 * Then the connection is lost: a datagram whose messages do not add up to its
 * end is dropped, and proves nothing
 */
TEST_F(ServerSessionFixture, ATruncatedDatagramKeepsNoSessionAlive) {
  accept();
  update(PROTOCOL_SILENCE_LIMIT - ONE_NANOSECOND);
  std::vector<std::byte> datagram = network::DatagramCodec::encode(
      sessionHeader(FIRST_SEQUENCE), network::Heartbeat{});
  datagram.pop_back();
  deliverBytes(std::move(datagram));
  update();

  update(ONE_NANOSECOND);

  EXPECT_EQ(phase(), Phase::Lost);
}

/**
 * Given a session in which a Heartbeat of the server waits in the link
 * When a frame one nanosecond longer than the silence limit ends
 * Then the session holds: the Heartbeat that arrived during that frame is
 * read before the silence is judged
 */
TEST_F(ServerSessionFixture, AFrameLongerThanTheTimeoutLosesNoSession) {
  accept();
  deliverInSession(network::Heartbeat{}, FIRST_SEQUENCE);

  update(PROTOCOL_SILENCE_LIMIT + ONE_NANOSECOND);

  EXPECT_EQ(phase(), Phase::Accepted);
}

/**
 * Given a client idle for the silence limit, then an attempt the server
 * accepts one request interval after it started
 * When the silence limit minus one nanosecond passes
 * Then the session holds: neither the idle time nor the wait of the attempt
 * counts as silence of the session
 */
TEST_F(ServerSessionFixture, KeepsASessionOpenedAfterAnIdleWait) {
  update(PROTOCOL_SILENCE_LIMIT);
  connect();
  update(PROTOCOL_REQUEST_INTERVAL);
  deliverInSession(
      network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER},
      ACCEPTANCE_SEQUENCE);
  update();

  update(PROTOCOL_SILENCE_LIMIT - ONE_NANOSECOND);

  EXPECT_EQ(phase(), Phase::Accepted);
}

/**
 * Given a session lost to the silence of its server, then a new attempt the
 * server accepts one request interval after it started
 * When the silence limit minus one nanosecond passes
 * Then the new session holds: the silence of the lost one does not carry over
 */
TEST_F(ServerSessionFixture, KeepsASessionOpenedAfterALostOne) {
  accept();
  update(PROTOCOL_SILENCE_LIMIT);
  connect();
  update(PROTOCOL_REQUEST_INTERVAL);
  deliverInSession(
      network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER},
      ACCEPTANCE_SEQUENCE);
  update();

  update(PROTOCOL_SILENCE_LIMIT - ONE_NANOSECOND);

  EXPECT_EQ(phase(), Phase::Accepted);
}

/**
 * Given a session the server opened, then sent nothing more
 * When the silence limit passes, then a heartbeat interval
 * Then nothing goes to the server: no Heartbeat, and no Disconnection, so
 * the server closes the session as timed out
 */
TEST_F(ServerSessionFixture, SendsNothingToALostServer) {
  accept();
  const std::size_t sentWhenAccepted = sent().size();

  update(PROTOCOL_SILENCE_LIMIT);
  update(PROTOCOL_HEARTBEAT_INTERVAL);

  EXPECT_EQ(sent().size(), sentWhenAccepted);
}

/**
 * Given a session with a room and a started game
 * When the server stays silent for the session timeout
 * Then the room and the start of the game are forgotten
 */
TEST_F(ServerSessionFixture, LosingTheServerForgetsTheSession) {
  accept();
  deliverInSession(network::GameStarted{}, FIRST_SEQUENCE);
  update();

  update(PROTOCOL_SILENCE_LIMIT);

  EXPECT_TRUE(waitingRoom().lobbyPlayers().empty());
  EXPECT_FALSE(waitingRoom().gameStarted());
}

/**
 * Given a session lost to the silence of the server
 * When a late copy of the acceptance of the server arrives
 * Then the connection stays lost: only a new attempt opens a session
 */
TEST_F(ServerSessionFixture, StaysLostWhenALateAcceptanceArrives) {
  accept();
  update(PROTOCOL_SILENCE_LIMIT);

  deliverInSession(
      network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER},
      FIRST_SEQUENCE);
  update();

  EXPECT_EQ(phase(), Phase::Lost);
}

/**
 * Given a session the server opened, then sent nothing more
 * When the silence limit passes
 * Then the journal names the server and the delay it stayed silent for
 */
TEST_F(ServerSessionLogFixture, LogsTheServerItLost) {
  accept();

  update(PROTOCOL_SILENCE_LIMIT);

  EXPECT_THAT(journal().linesHolding(LOST_SERVER_LINE_TEXT), SizeIs(1));
}

}  // namespace rtype::client
