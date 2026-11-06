#include <gtest/gtest.h>
#include <vector>
#include "Endpoint.hpp"
#include "PingAnswer.hpp"
#include "ServerPingProbeFixture.hpp"
#include "ServerPingProbeTestConstants.hpp"

using rtype::client::PingAnswer;
using rtype::client::testing::ANOTHER_SENT_AT;
using rtype::client::testing::ANSWER_ARRIVES_AFTER;
using rtype::client::testing::EXPECTED_PING;
using rtype::client::testing::REQUEST_DATAGRAM_SIZE;
using rtype::client::testing::SECOND_ANSWER_ARRIVES_AFTER;
using rtype::client::testing::ServerPingProbeFixture;
using rtype::network::Endpoint;

/**
 * Given a probe
 * When it measures Paris and Lyon
 * Then it sends one request to each, in order
 */
TEST_F(ServerPingProbeFixture, SendsOneRequestToEachServerOfTheRound) {
  measureParisAndLyon();

  ASSERT_EQ(link().sent().size(), 2U);
  EXPECT_EQ(link().sent().at(0).destination, paris());
  EXPECT_EQ(link().sent().at(1).destination, lyon());
}

/**
 * Given a probe
 * When it measures a server
 * Then its request is 106 bytes long, the only size a server answers
 */
TEST_F(ServerPingProbeFixture, SendsARequestOfTheSizeAServerAnswers) {
  measureParisAndLyon();

  EXPECT_EQ(link().sent().front().payload.size(), REQUEST_DATAGRAM_SIZE);
}

/**
 * Given a round to Paris and Lyon
 * When it starts
 * Then the route listens to those two servers and no others
 */
TEST_F(ServerPingProbeFixture, ListensToTheServersOfTheRound) {
  measureParisAndLyon();

  EXPECT_EQ(link().listenedToEach(), (std::vector<Endpoint>{paris(), lyon()}));
}

/**
 * Given a round sent at 1 s
 * When Paris answers at 1.030 s, having held the request 2 ms
 * Then its ping is 28 ms
 */
TEST_F(ServerPingProbeFixture, AnAnswerGivesTheRoundTripLessTheTimeHeld) {
  measureParisAndLyon();
  deliverAnswer(paris(), sentAtOfTheRound(), ANSWER_ARRIVES_AFTER);

  const std::vector<PingAnswer> answers = probe().answers();

  ASSERT_EQ(answers.size(), 1U);
  EXPECT_EQ(answers.front().server, paris());
  EXPECT_EQ(answers.front().roundTrip, EXPECTED_PING);
}

/**
 * Given a round to Paris and Lyon
 * When a server the round does not name answers it
 * Then no answer is read
 */
TEST_F(ServerPingProbeFixture, IgnoresAnAnswerOfAServerOutsideTheRound) {
  measureParisAndLyon();
  deliverAnswer(stranger(), sentAtOfTheRound(), ANSWER_ARRIVES_AFTER);

  EXPECT_TRUE(probe().answers().empty());
}

/**
 * Given a round to Paris and Lyon
 * When Paris answers with a sentAt the round never sent
 * Then no answer is read
 */
TEST_F(ServerPingProbeFixture, IgnoresAnAnswerEchoingAnotherSentAt) {
  measureParisAndLyon();
  deliverAnswer(paris(), ANOTHER_SENT_AT, ANSWER_ARRIVES_AFTER);

  EXPECT_TRUE(probe().answers().empty());
}

/**
 * Given Paris answered the round once
 * When a second answer of Paris arrives later
 * Then it is not read: the first answer gave the ping
 */
TEST_F(ServerPingProbeFixture, ASecondAnswerOfAServerMovesNoPing) {
  measureParisAndLyon();
  deliverAnswer(paris(), sentAtOfTheRound(), ANSWER_ARRIVES_AFTER);
  static_cast<void>(probe().answers());
  deliverAnswer(paris(), sentAtOfTheRound(), SECOND_ANSWER_ARRIVES_AFTER);

  EXPECT_TRUE(probe().answers().empty());
}

/**
 * Given Paris answered twice before the probe read anything
 * When the answers are read
 * Then only the first one counts
 */
TEST_F(ServerPingProbeFixture, TwoAnswersReadTogetherGiveTheFirstPing) {
  measureParisAndLyon();
  deliverAnswer(paris(), sentAtOfTheRound(), ANSWER_ARRIVES_AFTER);
  deliverAnswer(paris(), sentAtOfTheRound(), SECOND_ANSWER_ARRIVES_AFTER);

  const std::vector<PingAnswer> answers = probe().answers();

  ASSERT_EQ(answers.size(), 1U);
  EXPECT_EQ(answers.front().roundTrip, EXPECTED_PING);
}

/**
 * Given a round to Paris and Lyon
 * When Paris sends bytes that are no datagram of the protocol
 * Then nothing is read, and nothing throws
 */
TEST_F(ServerPingProbeFixture, DropsADatagramItCannotRead) {
  measureParisAndLyon();
  deliverGarbageFromParis();

  EXPECT_TRUE(probe().answers().empty());
}
