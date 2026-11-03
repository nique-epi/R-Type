#include <gtest/gtest.h>
#include "ConnectedPlayer.hpp"
#include "GameInstanceTestConstants.hpp"
#include "GameResultKeeperTestConstants.hpp"
#include "LeaderboardAnswer.hpp"
#include "LeaderboardDeskFixture.hpp"
#include "LeaderboardSamples.hpp"
#include "MessagePayload.hpp"
#include "SentDatagrams.hpp"

namespace rtype::server::testing {

using network::LeaderboardAnswer;

/**
 * Given a seated player
 * When the player asks for a leaderboard
 * Then the archive gets the request with the nickname and session of the player
 */
TEST_F(LeaderboardDeskFixture, HandsTheRequestToTheArchiveWithTheSession) {
  const ConnectedPlayer& player = seat(FIRST_PLAYER);

  ask(player);

  ASSERT_EQ(archive().asks().size(), 1U);
  EXPECT_EQ(archive().asks().at(0).request, someLeaderboardRequest());
  EXPECT_EQ(archive().asks().at(0).nickname, player.nickname);
  EXPECT_EQ(archive().asks().at(0).asker, player.endpoint);
  EXPECT_EQ(archive().asks().at(0).sessionToken, player.sessionToken);
}

/**
 * Given a player answered just now
 * When it asks again 29 ticks later, then 30 ticks later
 * Then the first is ignored and the second goes through: the two sides of the
 * interval of the protocol page
 */
TEST_F(LeaderboardDeskFixture, AnswersAPlayerOncePerInterval) {
  const ConnectedPlayer& player = seat(FIRST_PLAYER);
  ask(player);

  runTicks(TICKS_ONE_SHORT_OF_THE_INTERVAL);
  ask(player);
  EXPECT_EQ(archive().asks().size(), 1U);

  runTicks(1);
  ask(player);
  EXPECT_EQ(archive().asks().size(), 2U);
}

/**
 * Given a player answered just now
 * When another player asks in the same tick
 * Then the second one goes through: a flood costs its author only
 */
TEST_F(LeaderboardDeskFixture, CountsTheIntervalPerPlayer) {
  ask(seat(FIRST_PLAYER));

  ask(seat(SECOND_PLAYER));

  EXPECT_EQ(archive().asks().size(), 2U);
}

/**
 * Given a player who asked, and an answer ready
 * When a tick runs
 * Then the datagram goes to that player and holds that answer
 */
TEST_F(LeaderboardDeskFixture, SendsTheAnswerToThePlayerWhoAsked) {
  ask(seat(FIRST_PLAYER));
  const LeaderboardAnswer answer = emptyLeaderboardAnswer();
  archive().answerEveryAsk(network::encodeMessage(answer));

  runTicks(1);

  const auto received = sentTo(sent(), playerEndpoint(FIRST_PLAYER));
  ASSERT_EQ(received.size(), 1U);
  EXPECT_EQ(onlyMessageIn<LeaderboardAnswer>(received.at(0)), answer);
}

/**
 * Given a player who asked, then left
 * When the answer is ready
 * Then nothing is sent: nobody proves that address any more
 */
TEST_F(LeaderboardDeskFixture, DropsTheAnswerToAPlayerWhoLeft) {
  ask(seat(FIRST_PLAYER));
  leave(FIRST_PLAYER);
  archive().answerEveryAsk(network::encodeMessage(emptyLeaderboardAnswer()));

  runTicks(1);

  EXPECT_TRUE(sent().empty());
}

/**
 * Given a player who asked, then left, and another session from the same
 * address
 * When the answer is ready
 * Then nothing is sent: the answer belongs to the old session
 */
TEST_F(LeaderboardDeskFixture, DropsTheAnswerOfAnOlderSessionOfThatAddress) {
  ask(seat(FIRST_PLAYER));
  leave(FIRST_PLAYER);
  seatAgainUnderANewToken(FIRST_PLAYER);
  archive().answerEveryAsk(network::encodeMessage(emptyLeaderboardAnswer()));

  runTicks(1);

  EXPECT_TRUE(sent().empty());
}

}  // namespace rtype::server::testing
