#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "Logger.hpp"

using rtype::logging::Logger;
using rtype::logging::LogLevel;

namespace {

class CerrCapture {
 public:
  CerrCapture() : oldBuffer_(std::cerr.rdbuf()) {
    std::cerr.rdbuf(buffer_.rdbuf());
  }

  ~CerrCapture() { std::cerr.rdbuf(oldBuffer_); }

  CerrCapture(const CerrCapture&) = delete;
  CerrCapture& operator=(const CerrCapture&) = delete;
  CerrCapture(CerrCapture&&) = delete;
  CerrCapture& operator=(CerrCapture&&) = delete;

  std::string str() const { return buffer_.str(); }

 private:
  std::streambuf* oldBuffer_;
  std::stringstream buffer_;
};

class LoggerTest : public ::testing::Test {
 protected:
  void SetUp() override { savedLevel_ = Logger::level(); }
  void TearDown() override { Logger::setLevel(savedLevel_); }

 private:
  LogLevel savedLevel_{LogLevel::Info};
};

struct CountedValue {
  int* formattedCount;
};

std::ostream& operator<<(std::ostream& stream, const CountedValue& value) {
  ++*value.formattedCount;
  return stream << "counted";
}

std::vector<std::string> splitLines(const std::string& text) {
  std::vector<std::string> lines;
  std::istringstream stream(text);
  std::string line;
  while (std::getline(stream, line)) {
    lines.push_back(line);
  }
  return lines;
}

std::string standardErrorLinePattern(const std::string& label,
                                     const std::string& module,
                                     const std::string& body) {
  return R"((\x1b\[\d+m)?\[\d{2}:\d{2}:\d{2}\.\d{3}\] \[)" + label +
         R"(\] \[)" + module + R"(\] - )" + body + R"((\x1b\[0m)?)";
}

std::size_t countOccurrences(const std::string& text,
                             const std::string& needle) {
  std::size_t count = 0;
  std::size_t position = 0;
  while ((position = text.find(needle, position)) != std::string::npos) {
    ++count;
    position += 1;
  }
  return count;
}

}  // namespace

/**
 * Given the trace level and a logger named Network
 * When one info line is logged
 * Then standard error holds exactly one line in the documented format
 */
TEST_F(LoggerTest, EmitsTheDocumentedLineFormat) {
  Logger::setLevel(LogLevel::Trace);
  const CerrCapture capture;
  const Logger logger("Network");

  logger.info("hello world");

  const std::vector<std::string> lines = splitLines(capture.str());
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_TRUE(std::regex_match(
      lines[0],
      std::regex(standardErrorLinePattern("INFO ", "Network", "hello world"))));
}

/**
 * Given the info level
 * When a line is logged from arguments of several types
 * Then the body is their concatenation
 */
TEST_F(LoggerTest, ConcatenatesHeterogeneousArguments) {
  constexpr int width = 1920;
  constexpr int height = 1080;
  constexpr double durationInMilliseconds = 12.5;
  Logger::setLevel(LogLevel::Info);
  const CerrCapture capture;
  const Logger logger("Network");

  logger.info("received ", width, 'x', height, " in ", durationInMilliseconds,
              " ms");

  const std::string output = capture.str();
  EXPECT_NE(output.find("received 1920x1080 in 12.5 ms"), std::string::npos);
}

/**
 * Given the warn level
 * When one line is logged at each level
 * Then only the warn and error lines are written
 */
TEST_F(LoggerTest, LevelFilterDropsLowerSeverity) {
  Logger::setLevel(LogLevel::Warn);
  const CerrCapture capture;
  const Logger logger("Filter");

  logger.trace("trace-body");
  logger.debug("debug-body");
  logger.info("info-body");
  logger.warn("warn-body");
  logger.error("error-body");

  const std::string output = capture.str();
  EXPECT_EQ(output.find("trace-body"), std::string::npos);
  EXPECT_EQ(output.find("debug-body"), std::string::npos);
  EXPECT_EQ(output.find("info-body"), std::string::npos);
  EXPECT_NE(output.find("warn-body"), std::string::npos);
  EXPECT_NE(output.find("error-body"), std::string::npos);
}

/**
 * Given the silent level
 * When an error line is logged
 * Then nothing is written
 */
TEST_F(LoggerTest, SilentLevelDropsEverything) {
  Logger::setLevel(LogLevel::Silent);
  const CerrCapture capture;
  const Logger logger("Silent");

  logger.error("should be dropped");

  EXPECT_TRUE(capture.str().empty());
}

/**
 * Given the info level
 * When a debug line and an info line are logged
 * Then only the info line is written
 */
TEST_F(LoggerTest, DefaultRuntimeLevelHidesDebug) {
  Logger::setLevel(LogLevel::Info);
  const CerrCapture capture;
  const Logger logger("Filter");

  logger.debug("invisible");
  logger.info("visible");

  const std::string output = capture.str();
  EXPECT_EQ(output.find("invisible"), std::string::npos);
  EXPECT_NE(output.find("visible"), std::string::npos);
}

/**
 * Given the info level
 * When shouldLog is asked for every level
 * Then it is true from info upward only
 */
TEST_F(LoggerTest, ShouldLogMatchesLevel) {
  Logger::setLevel(LogLevel::Info);

  EXPECT_FALSE(Logger::shouldLog(LogLevel::Trace));
  EXPECT_FALSE(Logger::shouldLog(LogLevel::Debug));
  EXPECT_TRUE(Logger::shouldLog(LogLevel::Info));
  EXPECT_TRUE(Logger::shouldLog(LogLevel::Warn));
  EXPECT_TRUE(Logger::shouldLog(LogLevel::Error));
}

/**
 * Given the info level and a scoped timer lasting two milliseconds
 * When the scope ends
 * Then a line states the label and the elapsed time in milliseconds
 */
TEST_F(LoggerTest, ScopedTimerLogsOnDestruction) {
  Logger::setLevel(LogLevel::Info);
  const CerrCapture capture;
  const Logger logger("Timer");
  {
    auto timer = logger.scope("phase A");
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }

  const std::string output = capture.str();
  EXPECT_NE(output.find("[Timer]"), std::string::npos);
  EXPECT_NE(output.find("phase A took"), std::string::npos);
  EXPECT_NE(output.find(" ms"), std::string::npos);
}

/**
 * Given the error level and a scoped timer at the info level
 * When the scope ends
 * Then nothing is written
 */
TEST_F(LoggerTest, ScopedTimerSilentWhenFiltered) {
  Logger::setLevel(LogLevel::Error);
  const CerrCapture capture;
  const Logger logger("Timer");
  {
    auto timer = logger.scope("phase", LogLevel::Info);
  }

  EXPECT_TRUE(capture.str().empty());
}

/**
 * Given the info level and a scoped timer returned by value from scope
 * When the scope ends
 * Then exactly one line is written, the moved-from timer stays silent
 */
TEST_F(LoggerTest, ScopedTimerMovedFromInstanceIsSilent) {
  Logger::setLevel(LogLevel::Info);
  const CerrCapture capture;
  const Logger logger("Timer");
  {
    auto timer = logger.scope("once");
    (void)timer;
  }

  EXPECT_EQ(countOccurrences(capture.str(), "once took"), 1U);
}

/**
 * Given the info level
 * When a debug call receives a value that counts how often it is formatted
 * Then the value is never formatted, while an info call formats it once
 */
TEST_F(LoggerTest, FilteredCallDoesNotFormatItsArguments) {
  Logger::setLevel(LogLevel::Info);
  const CerrCapture capture;
  const Logger logger("Cost");
  int formattedCount = 0;

  logger.debug(CountedValue{&formattedCount});
  EXPECT_EQ(formattedCount, 0);

  logger.info(CountedValue{&formattedCount});
  EXPECT_EQ(formattedCount, 1);
}

/**
 * Given four threads logging fifty lines each
 * When they all finish
 * Then every line is whole and each (thread, message) pair appears exactly once
 */
TEST_F(LoggerTest, IsThreadSafeAcrossConcurrentWrites) {
  constexpr int threadCount = 4;
  constexpr int messagesPerThread = 50;
  Logger::setLevel(LogLevel::Info);
  const CerrCapture capture;
  const Logger logger("Worker");

  std::vector<std::thread> threads;
  threads.reserve(threadCount);
  for (int threadIndex = 0; threadIndex < threadCount; ++threadIndex) {
    threads.emplace_back([threadIndex, &logger] {
      for (int messageIndex = 0; messageIndex < messagesPerThread;
           ++messageIndex) {
        logger.info("thread=", threadIndex, " message=", messageIndex);
      }
    });
  }
  for (auto& thread : threads) {
    thread.join();
  }

  const std::vector<std::string> lines = splitLines(capture.str());
  EXPECT_EQ(lines.size(),
            static_cast<std::size_t>(threadCount * messagesPerThread));
  const std::regex pattern(standardErrorLinePattern(
      "INFO ", "Worker", R"(thread=(\d+) message=(\d+))"));
  std::set<std::pair<int, int>> seen;
  for (const std::string& line : lines) {
    std::smatch match;
    ASSERT_TRUE(std::regex_match(line, match, pattern)) << line;
    seen.emplace(std::stoi(match[2]), std::stoi(match[3]));
  }
  EXPECT_EQ(seen.size(),
            static_cast<std::size_t>(threadCount * messagesPerThread));
}
