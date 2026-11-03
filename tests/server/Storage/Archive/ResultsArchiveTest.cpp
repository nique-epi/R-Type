#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "LeaderboardAnswer.hpp"
#include "LeaderboardReply.hpp"
#include "LeaderboardRequest.hpp"
#include "MessagePayload.hpp"
#include "ResultsArchive.hpp"
#include "ResultsArchiveFixture.hpp"
#include "ResultsDatabase.hpp"
#include "ResultsDatabaseFixture.hpp"
#include "ResultsDatabaseTestConstants.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "StorageException.hpp"

namespace rtype::server::testing {

using network::LeaderboardAnswer;
using network::LeaderboardRequest;

/**
 * Given an open archive
 * When a result is recorded, then the archive closes at once
 * Then the result is in the file: closing finishes what waits
 */
TEST_F(ResultsArchiveFixture, SavesWhatWaitsBeforeItCloses) {
  ResultsArchive archive;
  archive.open(file());

  archive.record(normalGame({playerNamed(ACE, HIGH_SCORE)}, SOME_DATE));
  archive.close();

  const ResultsDatabase reopened{file()};
  EXPECT_EQ(reopened.leaderboard(normalBoard(), std::string{ACE}).rows.size(),
            1U);
}

/**
 * Given an open archive
 * When a result is recorded, then a player asks for that leaderboard
 * Then the answer holds the result and comes back with the session it was asked
 * in
 */
TEST_F(ResultsArchiveFixture,
       AnswersOnTheQueueOfTheAskAfterSavingWhatCameFirst) {
  ResultsArchive archive;
  archive.open(file());

  archive.record(normalGame({playerNamed(ACE, HIGH_SCORE)}, SOME_DATE));
  archive.ask(askFor(normalBoard()));

  const std::optional<LeaderboardReply> reply = nextReply();
  ASSERT_TRUE(reply.has_value());
  const LeaderboardReply received = reply.value_or(LeaderboardReply{});
  EXPECT_EQ(received.sessionToken, SESSION_TOKEN);
  const auto answer =
      rtype::network::decodeMessage<LeaderboardAnswer>(received.payload);
  ASSERT_EQ(answer.rows.size(), 1U);
  EXPECT_EQ(answer.rows.at(0).score, HIGH_SCORE);
}

/**
 * Given a result the database refuses, a nickname twice in one game
 * When it is recorded, then a good one is
 * Then the thread goes on, and only the good one is saved: a refused game
 * leaves no row
 */
TEST_F(ResultsArchiveFixture, GoesOnAfterAResultTheDatabaseRefuses) {
  ResultsArchive archive;
  archive.open(file());

  archive.record(normalGame(
      {playerNamed(BOB, LOW_SCORE), playerNamed(BOB, LOW_SCORE)}, SOME_DATE));
  archive.record(normalGame({playerNamed(ACE, HIGH_SCORE)}, SOME_DATE));
  archive.close();

  const ResultsDatabase reopened{file()};
  const LeaderboardAnswer answer =
      reopened.leaderboard(normalBoard(), std::string{ACE});
  ASSERT_EQ(answer.rows.size(), 1U);
  EXPECT_EQ(answer.rows.at(0).nickname, ACE);
}

/**
 * Given a stored row the protocol refuses, an empty nickname, on one
 * leaderboard
 * When a player asks for it, then for another leaderboard
 * Then the first ask gets no answer, and the thread still answers the second
 */
TEST_F(ResultsArchiveFixture, GoesOnAfterARowTheProtocolRefuses) {
  ResultsArchive archive;
  archive.open(file());
  archive.record(normalGame({playerNamed("", HIGH_SCORE)}, SOME_DATE));

  archive.ask(askFor(normalBoard()));
  archive.ask(askFor(LeaderboardRequest{.playerCount = THREE_PLAYERS,
                                        .difficulty = std::string{NORMAL}}));

  const std::optional<LeaderboardReply> reply = nextReply();
  ASSERT_TRUE(reply.has_value());
  const auto answer = rtype::network::decodeMessage<LeaderboardAnswer>(
      reply.value_or(LeaderboardReply{}).payload);
  EXPECT_EQ(answer.asked.playerCount, THREE_PLAYERS);
}

/**
 * Given a file that cannot be created
 * When the archive opens it
 * Then the error reaches the caller, and closing does nothing
 */
TEST_F(ResultsArchiveFixture, TellsTheCallerWhenItCannotOpenItsFile) {
  ResultsArchive archive;

  EXPECT_THROW(archive.open(file() / "not-a-folder" / "results.db"),
               rtype::storage::StorageException);

  archive.close();
}

/**
 * Given an archive never opened
 * When it closes
 * Then nothing happens
 */
TEST_F(ResultsArchiveFixture, ClosesWithoutHavingOpened) {
  ResultsArchive archive;

  EXPECT_NO_THROW(archive.close());
}

}  // namespace rtype::server::testing
