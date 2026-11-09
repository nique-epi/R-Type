#include <gtest/gtest.h>
#include "LagometerJournalTestConstants.hpp"
#include "LagometerJournalText.hpp"

namespace rtype::client {

using testing::JOURNAL_PERIOD_FIGURES;
using testing::JOURNAL_PERIOD_LINE;
using testing::JOURNAL_WITHOUT_PING;
using testing::JOURNAL_WITHOUT_PING_LINE;

/**
 * Given a period of 10 s with a ping of 85 ms and a jitter of 33 ms, 2 of 292
 * datagrams missed, 2 frames, 54 drawn past the newest state, 100 ms behind
 * When its line of the journal is written
 * Then it reads every figure, each rate rounded to the nearest
 */
TEST(LagometerJournalText, WritesThePeriodOfTheJournal) {
  EXPECT_EQ(lagometerJournalLine(JOURNAL_PERIOD_FIGURES), JOURNAL_PERIOD_LINE);
}

/**
 * Given a period of 1.5 s before any round trip, a frame drawn 33 ms ahead of
 * the server
 * When its line of the journal is written
 * Then the ping reads --, and the delay keeps its sign
 */
TEST(LagometerJournalText, WritesAJournalPeriodWithoutAPing) {
  EXPECT_EQ(lagometerJournalLine(JOURNAL_WITHOUT_PING),
            JOURNAL_WITHOUT_PING_LINE);
}

}  // namespace rtype::client
