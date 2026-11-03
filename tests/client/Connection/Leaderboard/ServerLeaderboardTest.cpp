#include <gtest/gtest.h>
#include "LeaderboardPage.hpp"
#include "LeaderboardPhase.hpp"
#include "LeaderboardRequest.hpp"
#include "ServerLeaderboardFixture.hpp"
#include "ServerLeaderboardTestConstants.hpp"
#include "ServerSessionFixtures.hpp"
#include "ServerSessionTestConstants.hpp"

namespace rtype::client {

using testing::ASK_INTERVAL;
using testing::ASKED_DIFFICULTY;
using testing::ASKED_PILOTS;
using testing::FIRST_SEQUENCE;
using testing::FRAMES_UNTIL_GIVING_UP;
using testing::GIVE_UP_TIME;
using testing::onlyMessageOf;
using testing::OTHER_DIFFICULTY;
using testing::REQUESTS_BEFORE_GIVING_UP;
using testing::SECOND_SEQUENCE;
using testing::ServerLeaderboardFixture;
using testing::SHORT_FRAME;
using testing::SMALLEST_TIME;
using testing::TOP_NICKNAME;

/**
 * Given no session
 * When the leaderboard is asked for
 * Then it says so, and nothing is sent
 */
TEST_F(ServerLeaderboardFixture, WithoutASessionItSaysNotOnServer) {
  leaderboard().ask(askedRequest());

  EXPECT_EQ(leaderboard().page().phase, LeaderboardPhase::NotOnServer);
  EXPECT_EQ(sentRequestCount(), 0);
}

/**
 * Given a leaderboard asked for without a session
 * When a session opens
 * Then the request goes out, and the page says it is asking
 */
TEST_F(ServerLeaderboardFixture, ARequestWithoutASessionWaitsForOne) {
  leaderboard().ask(askedRequest());

  connectAndGetAccepted();

  EXPECT_EQ(leaderboard().page().phase, LeaderboardPhase::Asking);
  EXPECT_EQ(sentRequestCount(), 1);
}

/**
 * Given a leaderboard asked for without a session, and an attempt dropped, as a
 * connection screen does when it opens
 * When a session opens
 * Then the request goes out all the same
 */
TEST_F(ServerLeaderboardFixture, ADroppedAttemptKeepsTheRequest) {
  leaderboard().ask(askedRequest());
  session().cancel();

  connectAndGetAccepted();

  EXPECT_EQ(sentRequestCount(), 1);
}

/**
 * Given a leaderboard asked for without a session, then a screen that left
 * When a session opens
 * Then nothing goes out: nobody reads the answer
 */
TEST_F(ServerLeaderboardFixture, StoppingWithoutASessionForgetsTheRequest) {
  leaderboard().ask(askedRequest());
  leaderboard().stopAsking();

  connectAndGetAccepted();

  EXPECT_EQ(leaderboard().page().phase, LeaderboardPhase::NotOnServer);
  EXPECT_EQ(sentRequestCount(), 0);
}

/**
 * Given a session
 * When the leaderboard is asked for
 * Then the request goes out at once, for that leaderboard
 */
TEST_F(ServerLeaderboardFixture, AskingSendsTheRequestAtOnce) {
  connectAndGetAccepted();

  leaderboard().ask(askedRequest());

  EXPECT_EQ(leaderboard().page().phase, LeaderboardPhase::Asking);
  const auto request =
      onlyMessageOf<network::LeaderboardRequest>(sent().back());
  EXPECT_EQ(request.playerCount, ASKED_PILOTS);
  EXPECT_EQ(request.difficulty, ASKED_DIFFICULTY);
}

/**
 * Given a request without an answer
 * When just under the interval goes by
 * Then nothing more is sent
 */
TEST_F(ServerLeaderboardFixture, TheRequestWaitsItsInterval) {
  connectAndGetAccepted();
  leaderboard().ask(askedRequest());

  update(ASK_INTERVAL - SMALLEST_TIME);

  EXPECT_EQ(sentRequestCount(), 1);
}

/**
 * Given a request without an answer
 * When the interval goes by
 * Then it is sent again
 */
TEST_F(ServerLeaderboardFixture, AnUnansweredRequestIsSentAgain) {
  connectAndGetAccepted();
  leaderboard().ask(askedRequest());

  update(ASK_INTERVAL);

  EXPECT_EQ(sentRequestCount(), 2);
}

/**
 * Given a request
 * When the answer for that leaderboard arrives
 * Then the page lists it under a new revision, and no request goes out again
 */
TEST_F(ServerLeaderboardFixture, TheAnswerFillsThePage) {
  connectAndGetAccepted();
  leaderboard().ask(askedRequest());
  const auto revision = leaderboard().page().revision;

  deliverInSession(answerFor(ASKED_DIFFICULTY), FIRST_SEQUENCE);
  update(ASK_INTERVAL);

  const LeaderboardPage& page = leaderboard().page();
  EXPECT_EQ(page.phase, LeaderboardPhase::Answered);
  ASSERT_EQ(page.rows.size(), 1);
  EXPECT_EQ(page.rows.front().nickname, TOP_NICKNAME);
  EXPECT_NE(page.revision, revision);
  EXPECT_EQ(sentRequestCount(), 1);
}

/**
 * Given a request for a leaderboard
 * When an answer for another one arrives, late, after a filter changed
 * Then the page stays as it is: it never shows what the player no longer asks
 */
TEST_F(ServerLeaderboardFixture, AnAnswerToAnotherFilterIsIgnored) {
  connectAndGetAccepted();
  leaderboard().ask(askedRequest());

  deliverInSession(answerFor(OTHER_DIFFICULTY), FIRST_SEQUENCE);
  update();

  EXPECT_EQ(leaderboard().page().phase, LeaderboardPhase::Asking);
}

/**
 * Given a request already answered
 * When a copy of the answer arrives
 * Then the page keeps its revision
 */
TEST_F(ServerLeaderboardFixture, ACopyOfTheAnswerChangesNothing) {
  connectAndGetAccepted();
  leaderboard().ask(askedRequest());
  deliverInSession(answerFor(ASKED_DIFFICULTY), FIRST_SEQUENCE);
  update();
  const auto revision = leaderboard().page().revision;

  deliverInSession(answerFor(ASKED_DIFFICULTY), SECOND_SEQUENCE);
  update();

  EXPECT_EQ(leaderboard().page().revision, revision);
}

/**
 * Given a request the server never answers
 * When just under the time to give up goes by
 * Then the client still asks
 */
TEST_F(ServerLeaderboardFixture, TheClientAsksUntilItGivesUp) {
  connectAndGetAccepted();
  leaderboard().ask(askedRequest());

  update(GIVE_UP_TIME - SMALLEST_TIME);

  EXPECT_EQ(leaderboard().page().phase, LeaderboardPhase::Asking);
}

/**
 * Given a request the server never answers
 * When the time to give up goes by, then an interval more
 * Then the page says so, and nothing more is sent
 */
TEST_F(ServerLeaderboardFixture, AfterFiveSecondsTheClientGivesUp) {
  connectAndGetAccepted();
  leaderboard().ask(askedRequest());

  update(GIVE_UP_TIME);
  update(ASK_INTERVAL);

  EXPECT_EQ(leaderboard().page().phase, LeaderboardPhase::NoAnswer);
  EXPECT_EQ(sentRequestCount(), 1);
}

/**
 * Given a request the server never answers
 * When short frames go by until the client gives up
 * Then the requests went out every half second, ten in all
 */
TEST_F(ServerLeaderboardFixture, TheClientSendsTenRequestsAtMost) {
  connectAndGetAccepted();
  leaderboard().ask(askedRequest());

  for (int frame = 0; frame < FRAMES_UNTIL_GIVING_UP; ++frame) {
    update(SHORT_FRAME);
  }

  EXPECT_EQ(leaderboard().page().phase, LeaderboardPhase::NoAnswer);
  EXPECT_EQ(sentRequestCount(), REQUESTS_BEFORE_GIVING_UP);
}

/**
 * Given a client that gave up
 * When the player asks again, and an interval goes by
 * Then the client still asks: it has its whole time again
 */
TEST_F(ServerLeaderboardFixture, AskingAgainAfterGivingUpAsksAgain) {
  connectAndGetAccepted();
  leaderboard().ask(askedRequest());
  update(GIVE_UP_TIME);

  leaderboard().ask(askedRequest());
  update(ASK_INTERVAL);

  EXPECT_EQ(leaderboard().page().phase, LeaderboardPhase::Asking);
  EXPECT_EQ(sentRequestCount(), 3);
}

/**
 * Given a client that gave up
 * When the answer arrives after all
 * Then the page shows it
 */
TEST_F(ServerLeaderboardFixture, AnAnswerAfterGivingUpStillShows) {
  connectAndGetAccepted();
  leaderboard().ask(askedRequest());
  update(GIVE_UP_TIME);

  deliverInSession(answerFor(ASKED_DIFFICULTY), FIRST_SEQUENCE);
  update();

  EXPECT_EQ(leaderboard().page().phase, LeaderboardPhase::Answered);
}

/**
 * Given a request without an answer
 * When the screen stops asking, and the interval goes by
 * Then nothing more is sent
 */
TEST_F(ServerLeaderboardFixture, StoppingEndsTheRequests) {
  connectAndGetAccepted();
  leaderboard().ask(askedRequest());

  leaderboard().stopAsking();
  update(ASK_INTERVAL);

  EXPECT_EQ(sentRequestCount(), 1);
}

/**
 * Given an answered leaderboard
 * When the session closes
 * Then the page forgets its rows, and says there is no server
 */
TEST_F(ServerLeaderboardFixture, TheEndOfTheSessionForgetsThePage) {
  connectAndGetAccepted();
  leaderboard().ask(askedRequest());
  deliverInSession(answerFor(ASKED_DIFFICULTY), FIRST_SEQUENCE);
  update();

  session().cancel();

  EXPECT_EQ(leaderboard().page().phase, LeaderboardPhase::NotOnServer);
  EXPECT_TRUE(leaderboard().page().rows.empty());
}

}  // namespace rtype::client
