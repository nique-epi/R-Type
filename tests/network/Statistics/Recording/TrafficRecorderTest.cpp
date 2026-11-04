#include <gtest/gtest.h>
#include <filesystem>
#include <optional>
#include <string>
#include "DebugJournal.hpp"
#include "MessageType.hpp"
#include "NetworkException.hpp"
#include "TemporaryFolder.hpp"
#include "TimeTestConstants.hpp"
#include "TrafficCounters.hpp"
#include "TrafficFileTestConstants.hpp"
#include "TrafficFixtures.hpp"
#include "TrafficRecorder.hpp"
#include "TrafficTestConstants.hpp"

using rtype::engine::testing::ONE_SECOND;
using rtype::network::MessageType;
using rtype::network::TrafficCounters;
using rtype::network::TrafficFileOpenException;
using rtype::network::TrafficRecorder;
using rtype::network::testing::datagramHolding;
using rtype::network::testing::loopbackOnPort;
using rtype::network::testing::textOf;

/**
 * Given a recorder with a file, and a datagram counted
 * When a nanosecond less than a second passed
 * Then the file holds no row yet
 */
TEST(TrafficRecorder, WritesNoRowBeforeASecond) {
  const TemporaryFolder folder;
  const std::filesystem::path file = folder.path() / TRAFFIC_FILE_NAME;
  TrafficCounters counters;
  TrafficRecorder recorder{counters, file, RECORDER_START};
  counters.countSent(loopbackOnPort(PEER_PORT),
                     datagramHolding(MessageType::Heartbeat, EMPTY_PAYLOAD));

  recorder.advance(RECORDER_START + ONE_SECOND - ONE_NANOSECOND);

  EXPECT_EQ(textOf(file), HEADER_LINE);
}

/**
 * Given a recorder started at five seconds on its clock, and a datagram counted
 * When a second passed
 * Then the row gives the time since the start, not the time of the clock
 */
TEST(TrafficRecorder, ARowGivesTheTimeSinceTheStart) {
  const TemporaryFolder folder;
  const std::filesystem::path file = folder.path() / TRAFFIC_FILE_NAME;
  TrafficCounters counters;
  TrafficRecorder recorder{counters, file, RECORDER_START};
  counters.countSent(loopbackOnPort(PEER_PORT),
                     datagramHolding(MessageType::Heartbeat, EMPTY_PAYLOAD));

  recorder.advance(RECORDER_START + ONE_SECOND);

  EXPECT_EQ(textOf(file), std::string{HEADER_LINE} +
                              "1.000,others,sent,all,1,25\n"
                              "1.000,others,sent,Heartbeat,1,3\n");
}

/**
 * Given a recorder that just wrote its rows
 * When half a second passes, then the rest of a second
 * Then it writes again only then
 */
TEST(TrafficRecorder, WritesOnceEachSecond) {
  const TemporaryFolder folder;
  const std::filesystem::path file = folder.path() / TRAFFIC_FILE_NAME;
  TrafficCounters counters;
  TrafficRecorder recorder{counters, file, RECORDER_START};
  counters.countSent(loopbackOnPort(PEER_PORT),
                     datagramHolding(MessageType::Heartbeat, EMPTY_PAYLOAD));
  recorder.advance(RECORDER_START + ONE_SECOND);
  const std::string afterTheFirst = textOf(file);

  recorder.advance(RECORDER_START + ONE_SECOND + HALF_A_SECOND);
  EXPECT_EQ(textOf(file), afterTheFirst);

  recorder.advance(RECORDER_START + ONE_SECOND + ONE_SECOND);
  EXPECT_NE(textOf(file), afterTheFirst);
}

/**
 * Given a recorder that wrote nothing yet, and a datagram counted
 * When it finishes, before a second passed
 * Then the rows are written anyway: they are the last of the run
 */
TEST(TrafficRecorder, FinishWritesTheLastRows) {
  const TemporaryFolder folder;
  const std::filesystem::path file = folder.path() / TRAFFIC_FILE_NAME;
  TrafficCounters counters;
  TrafficRecorder recorder{counters, file, RECORDER_START};
  counters.countSent(loopbackOnPort(PEER_PORT),
                     datagramHolding(MessageType::Heartbeat, EMPTY_PAYLOAD));

  recorder.finish(RECORDER_START + HALF_A_SECOND);

  EXPECT_EQ(textOf(file), std::string{HEADER_LINE} +
                              "0.500,others,sent,all,1,25\n"
                              "0.500,others,sent,Heartbeat,1,3\n");
}

/**
 * Given a recorder without a file
 * When a nanosecond less than ten seconds passed, then ten seconds
 * Then the journal holds no line, then one
 */
TEST(TrafficRecorder, WritesALineEveryTenSecondsWithoutAFile) {
  const DebugJournal journal;
  const TrafficCounters counters;
  TrafficRecorder recorder{counters, std::nullopt, RECORDER_START};

  recorder.advance(RECORDER_START + TEN_SECONDS - ONE_NANOSECOND);
  EXPECT_TRUE(journal.linesHolding(TRAFFIC_LINE_MARK).empty());

  recorder.advance(RECORDER_START + TEN_SECONDS);
  EXPECT_EQ(journal.linesHolding(TRAFFIC_LINE_MARK).size(), 1U);
}

/**
 * Given a datagram counted before the recorder started, and one during each of
 * its first two lines
 * When the second line is written
 * Then it only counts the datagram of its own ten seconds
 */
TEST(TrafficRecorder, EachLineCountsSinceThePreviousOne) {
  const DebugJournal journal;
  TrafficCounters counters;
  const auto datagram = datagramHolding(MessageType::Heartbeat, EMPTY_PAYLOAD);
  counters.countSent(loopbackOnPort(PEER_PORT), datagram);
  TrafficRecorder recorder{counters, std::nullopt, RECORDER_START};
  counters.countSent(loopbackOnPort(PEER_PORT), datagram);
  recorder.advance(RECORDER_START + TEN_SECONDS);
  counters.countSent(loopbackOnPort(PEER_PORT), datagram);

  recorder.advance(RECORDER_START + TEN_SECONDS + TEN_SECONDS);

  const auto lines = journal.linesHolding(TRAFFIC_LINE_MARK);
  ASSERT_EQ(lines.size(), 2U);
  EXPECT_NE(lines.front().find("sent 1 datagrams, 25 bytes"),
            std::string::npos);
  EXPECT_NE(lines.back().find("sent 1 datagrams, 25 bytes"), std::string::npos);
}

/**
 * Given a path that is a folder
 * When a recorder is built on it
 * Then it refuses: an option asked for never fails silently
 */
TEST(TrafficRecorder, RefusesAFileItCannotOpen) {
  const TemporaryFolder folder;
  const TrafficCounters counters;

  EXPECT_THROW(
      const TrafficRecorder recorder(counters, folder.path(), RECORDER_START),
      TrafficFileOpenException);
}

/**
 * Given a recorder on a device that refuses every write
 * When two seconds pass
 * Then the journal says so once, and nothing fails: a game goes on
 */
TEST(TrafficRecorder, AFileThatCannotBeWrittenIsToldOnceThenLeft) {
  if (!std::filesystem::exists(FULL_DISK_DEVICE)) {
    GTEST_SKIP() << "this system has no device that refuses every write";
  }
  const DebugJournal journal;
  const TrafficCounters counters;
  TrafficRecorder recorder{counters, std::filesystem::path{FULL_DISK_DEVICE},
                           RECORDER_START};

  recorder.advance(RECORDER_START + ONE_SECOND);
  recorder.advance(RECORDER_START + ONE_SECOND + ONE_SECOND);

  EXPECT_EQ(journal.linesHolding(UNWRITABLE_FILE_MARK).size(), 1U);
}
