#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <future>
#include <string>
#include <thread>
#include <vector>
#include "GatedJournalDestination.hpp"
#include "JournalWriter.hpp"
#include "JournalWriterTestConstants.hpp"
#include "LogLevel.hpp"
#include "LoggingConstants.hpp"
#include "ReceivedJournalLine.hpp"

using rtype::logging::JOURNAL_QUEUE_CAPACITY;
using rtype::logging::JournalWriter;
using rtype::logging::LogLevel;

namespace {

/** @brief Queues the line that reads @p number. */
void queueNumber(JournalWriter& writer, std::size_t number) {
  writer.queue(LogLevel::Info, WRITER_TEST_LOGGER_NAME, std::to_string(number));
}

/** @returns The texts of @p lines, in order. */
std::vector<std::string> textsOf(
    const std::vector<ReceivedJournalLine>& lines) {
  std::vector<std::string> texts;
  texts.reserve(lines.size());
  for (const ReceivedJournalLine& line : lines) {
    texts.push_back(line.text);
  }
  return texts;
}

/** @returns "0" to "count - 1", in order. */
std::vector<std::string> numbersBelow(std::size_t count) {
  std::vector<std::string> numbers;
  numbers.reserve(count);
  for (std::size_t number = 0; number < count; ++number) {
    numbers.push_back(std::to_string(number));
  }
  return numbers;
}

/** @returns The sum of the lines lost before each of @p lines. */
std::size_t lostLinesOf(const std::vector<ReceivedJournalLine>& lines) {
  std::size_t lost = 0;
  for (const ReceivedJournalLine& line : lines) {
    lost += line.lostBefore;
  }
  return lost;
}

}  // namespace

/**
 * Given a journal whose thread is stuck on its first line, as on a disk that
 * does not answer
 * When 10 000 more lines are queued, more than the queue holds
 * Then every call returns while the thread is still stuck
 */
TEST(JournalWriterTest, ACallerDoesNotWaitForAStuckDestination) {
  GatedJournalDestination gate;
  JournalWriter writer{gate.destination()};
  gate.close();
  queueNumber(writer, 0);
  gate.waitForALineAtTheGate();

  const std::future<void> calls = std::async(std::launch::async, [&writer] {
    for (std::size_t number = 1;
         number <= JOURNAL_QUEUE_CAPACITY + LINES_PAST_THE_QUEUE; ++number) {
      queueNumber(writer, number);
    }
  });
  const std::future_status status = calls.wait_for(QUEUE_CALLS_DEADLINE);
  gate.open();

  EXPECT_EQ(status, std::future_status::ready);
}

/**
 * Given a journal whose thread is stuck on its first line
 * When more lines are queued than the queue holds, then the thread goes on
 * Then the lines that found a place are written in order, the later ones are
 * dropped, and the count of the dropped ones comes with the next line
 */
TEST(JournalWriterTest, AFullQueueKeepsTheFirstLinesAndCountsTheRest) {
  GatedJournalDestination gate;
  JournalWriter writer{gate.destination()};
  gate.close();
  queueNumber(writer, 0);
  gate.waitForALineAtTheGate();
  for (std::size_t number = 1;
       number <= JOURNAL_QUEUE_CAPACITY + LINES_PAST_THE_QUEUE; ++number) {
    queueNumber(writer, number);
  }

  gate.open();
  writer.flush();

  const std::vector<ReceivedJournalLine> lines = gate.lines();
  EXPECT_EQ(textsOf(lines), numbersBelow(JOURNAL_QUEUE_CAPACITY + 1));
  EXPECT_EQ(lostLinesOf(lines), LINES_PAST_THE_QUEUE);
}

/**
 * Given a journal whose thread is stuck, with lines queued behind it
 * When the journal is flushed from another thread once the thread goes on
 * Then every line queued before the flush was written when it returns
 */
TEST(JournalWriterTest, FlushWaitsForTheLinesQueuedBeforeIt) {
  GatedJournalDestination gate;
  JournalWriter writer{gate.destination()};
  gate.close();
  for (std::size_t number = 0; number < FEW_LINES; ++number) {
    queueNumber(writer, number);
  }
  gate.waitForALineAtTheGate();
  std::thread opener{[&gate] {
    std::this_thread::sleep_for(STUCK_DESTINATION_DELAY);
    gate.open();
  }};

  writer.flush();

  EXPECT_EQ(textsOf(gate.lines()), numbersBelow(FEW_LINES));
  opener.join();
}

/**
 * Given a journal whose thread is stuck
 * When a line is queued, and the thread goes on 200 ms later
 * Then the time of the line is the time of the call, not of the writing
 */
TEST(JournalWriterTest, TheTimeOfALineIsTheTimeOfItsCall) {
  GatedJournalDestination gate;
  JournalWriter writer{gate.destination()};
  gate.close();
  const auto beforeTheCall = std::chrono::system_clock::now();
  queueNumber(writer, 0);
  const auto afterTheCall = std::chrono::system_clock::now();
  gate.waitForALineAtTheGate();
  std::this_thread::sleep_for(STUCK_DESTINATION_DELAY);

  gate.open();
  writer.flush();

  const std::vector<ReceivedJournalLine> lines = gate.lines();
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_GE(lines.front().time, beforeTheCall);
  EXPECT_LE(lines.front().time, afterTheCall);
}

/**
 * Given lines waiting behind a stuck thread, which goes on 200 ms later
 * When the journal is closed meanwhile
 * Then every waiting line was written when close() returns, and a line queued
 * afterwards starts the thread again
 */
TEST(JournalWriterTest, CloseWritesTheWaitingLinesAndALaterLineStartsAgain) {
  GatedJournalDestination gate;
  JournalWriter writer{gate.destination()};
  gate.close();
  for (std::size_t number = 0; number < FEW_LINES; ++number) {
    queueNumber(writer, number);
  }
  gate.waitForALineAtTheGate();
  std::thread opener{[&gate] {
    std::this_thread::sleep_for(STUCK_DESTINATION_DELAY);
    gate.open();
  }};

  writer.close();
  const std::vector<std::string> writtenWhenClosed = textsOf(gate.lines());
  opener.join();
  queueNumber(writer, FEW_LINES);
  writer.flush();

  EXPECT_EQ(writtenWhenClosed, numbersBelow(FEW_LINES));
  EXPECT_EQ(textsOf(gate.lines()), numbersBelow(FEW_LINES + 1));
}

/**
 * Given four threads that each queue 500 numbered lines at once
 * When the journal is flushed
 * Then the lines of each thread were written in the order it queued them
 */
TEST(JournalWriterTest, TheLinesOfAThreadKeepTheirOrder) {
  GatedJournalDestination gate;
  JournalWriter writer{gate.destination()};
  std::vector<std::thread> threads;
  threads.reserve(QUEUING_THREADS);
  for (std::size_t thread = 0; thread < QUEUING_THREADS; ++thread) {
    threads.emplace_back([&writer, thread] {
      for (std::size_t line = 0; line < LINES_PER_THREAD; ++line) {
        queueNumber(writer, (thread * LINES_PER_THREAD) + line);
      }
    });
  }
  for (std::thread& thread : threads) {
    thread.join();
  }

  writer.flush();

  std::vector<std::size_t> nextOfThread(QUEUING_THREADS, 0);
  for (const ReceivedJournalLine& line : gate.lines()) {
    const std::size_t number = std::stoul(line.text);
    const std::size_t thread = number / LINES_PER_THREAD;
    EXPECT_EQ(number % LINES_PER_THREAD, nextOfThread.at(thread));
    nextOfThread.at(thread) = (number % LINES_PER_THREAD) + 1;
  }
  EXPECT_EQ(nextOfThread,
            std::vector<std::size_t>(QUEUING_THREADS, LINES_PER_THREAD));
}
