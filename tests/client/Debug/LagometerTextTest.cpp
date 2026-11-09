#include <gtest/gtest.h>
#include <array>
#include <string>
#include "LagometerConstants.hpp"
#include "LagometerFigures.hpp"
#include "LagometerJournalTestConstants.hpp"
#include "LagometerTestConstants.hpp"
#include "LagometerText.hpp"
#include "RoundTripFigures.hpp"

namespace rtype::client {

using testing::BYTES_AT_HALF_A_KILOBYTE;
using testing::BYTES_BELOW_HALF_A_KILOBYTE;
using testing::EMPTY_BUFFER_LINE;
using testing::EMPTY_LOSS_LINE;
using testing::EMPTY_PING_LINE;
using testing::EMPTY_TRAFFIC_LINE;
using testing::FULL_BUFFER_LINE;
using testing::FULL_LOSS_LINE;
using testing::FULL_PING_LINE;
using testing::FULL_TRAFFIC_LINE;
using testing::JOURNAL_PERIOD_FIGURES;
using testing::JOURNAL_PERIOD_LINE;
using testing::JOURNAL_WITHOUT_PING;
using testing::JOURNAL_WITHOUT_PING_LINE;
using testing::NINE_LATE_FRAMES_PER_SECOND;
using testing::SIXTY_FRAMES_PER_SECOND;
using testing::SOME_LOSS_PERCENT;
using testing::UNROUNDED_DISPLAY_DELAY;
using testing::UNROUNDED_JITTER;
using testing::UNROUNDED_ROUND_TRIP;

/**
 * Given figures of which nothing is known yet
 * When the lines of the panel are written
 * Then each unknown figure reads --, and no loss reads 0 %
 */
TEST(LagometerText, AnUnknownFigureReadsTwoDashes) {
  const auto lines = lagometerTextLines(LagometerFigures{});

  EXPECT_EQ(lines, (std::array<std::string, LAGOMETER_TEXT_LINE_COUNT>{
                       EMPTY_PING_LINE, EMPTY_LOSS_LINE, EMPTY_TRAFFIC_LINE,
                       EMPTY_BUFFER_LINE}));
}

/**
 * Given a round trip of 41.6 ms with a jitter of 3.4 ms, 2 % lost, 60 frames
 * per second, 12499 bytes received and 12500 sent per second, a display
 * 123.4 ms behind the server and 9 late frames per second
 * When the lines of the panel are written
 * Then they read 42 MS, JIT 3, 2 %, 60 FPS, 12 KB in, 13 KB out, BUF 123 MS
 * and LATE 9
 */
TEST(LagometerText, WritesEachFigureRoundedToTheNearest) {
  const LagometerFigures figures{
      .roundTrip = network::RoundTripFigures{.roundTrip = UNROUNDED_ROUND_TRIP,
                                             .jitter = UNROUNDED_JITTER},
      .lossPercent = SOME_LOSS_PERCENT,
      .framesPerSecond = SIXTY_FRAMES_PER_SECOND,
      .bytesReceivedPerSecond = BYTES_BELOW_HALF_A_KILOBYTE,
      .bytesSentPerSecond = BYTES_AT_HALF_A_KILOBYTE,
      .displayDelay = UNROUNDED_DISPLAY_DELAY,
      .pastTheNewestPerSecond = NINE_LATE_FRAMES_PER_SECOND};

  const auto lines = lagometerTextLines(figures);

  EXPECT_EQ(lines, (std::array<std::string, LAGOMETER_TEXT_LINE_COUNT>{
                       FULL_PING_LINE, FULL_LOSS_LINE, FULL_TRAFFIC_LINE,
                       FULL_BUFFER_LINE}));
}

/**
 * Given a period of 10 s with a ping of 85 ms and a jitter of 33 ms, 2 of 292
 * datagrams missed, 2 frames, 54 drawn past the newest state, 100 ms behind
 * When its line of the journal is written
 * Then it reads every figure, each rate rounded to the nearest
 */
TEST(LagometerText, WritesThePeriodOfTheJournal) {
  EXPECT_EQ(lagometerJournalLine(JOURNAL_PERIOD_FIGURES), JOURNAL_PERIOD_LINE);
}

/**
 * Given a period of 1.5 s before any round trip, a frame drawn 33 ms ahead of
 * the server
 * When its line of the journal is written
 * Then the ping reads --, and the delay keeps its sign
 */
TEST(LagometerText, WritesAJournalPeriodWithoutAPing) {
  EXPECT_EQ(lagometerJournalLine(JOURNAL_WITHOUT_PING),
            JOURNAL_WITHOUT_PING_LINE);
}

}  // namespace rtype::client
