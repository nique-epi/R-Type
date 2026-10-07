#include <gtest/gtest.h>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <optional>
#include <ostream>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>
#if defined(_WIN32)
#include <memory>
#endif
#include "JournalFile.hpp"
#include "Logger.hpp"
#include "LoggingException.hpp"

using rtype::logging::LogFileOpenException;
using rtype::logging::Logger;
using rtype::logging::LogLevel;
using rtype::logging::LogOutput;

namespace {

constexpr std::size_t hourPrefixBufferSize = 16;

std::optional<std::string> readTimezone() {
#if defined(_WIN32)
  char* rawValue = nullptr;
  std::size_t length = 0;
  if (_dupenv_s(&rawValue, &length, "TZ") != 0) {
    return std::nullopt;
  }
  const std::unique_ptr<char, decltype(&std::free)> owned(rawValue, &std::free);
  if (owned == nullptr) {
    return std::nullopt;
  }
  return std::string(owned.get());
#else
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  const char* value = std::getenv("TZ");
  if (value == nullptr) {
    return std::nullopt;
  }
  return std::string(value);
#endif
}

void writeTimezone(const std::optional<std::string>& value) {
#if defined(_WIN32)
  _putenv_s("TZ", value.value_or("").c_str());
  _tzset();
#else
  if (value.has_value()) {
    // NOLINTNEXTLINE(concurrency-mt-unsafe,misc-include-cleaner)
    setenv("TZ", value->c_str(), 1);
  } else {
    // NOLINTNEXTLINE(concurrency-mt-unsafe,misc-include-cleaner)
    unsetenv("TZ");
  }
  // NOLINTNEXTLINE(misc-include-cleaner)
  tzset();
#endif
}

class TimezoneOverride {
 public:
  explicit TimezoneOverride(const std::string& value)
      : previous_(readTimezone()) {
    writeTimezone(value);
  }

  ~TimezoneOverride() { writeTimezone(previous_); }

  TimezoneOverride(const TimezoneOverride&) = delete;
  TimezoneOverride& operator=(const TimezoneOverride&) = delete;
  TimezoneOverride(TimezoneOverride&&) = delete;
  TimezoneOverride& operator=(TimezoneOverride&&) = delete;

 private:
  std::optional<std::string> previous_;
};

std::string universalHourPrefix() {
  const std::time_t now =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::tm universalTime{};
#if defined(_WIN32)
  (void)gmtime_s(&universalTime, &now);
#else
  (void)gmtime_r(&now, &universalTime);
#endif
  std::array<char, hourPrefixBufferSize> buffer{};
  (void)std::strftime(buffer.data(), buffer.size(), "%Y-%m-%dT%H",
                      &universalTime);
  return buffer.data();
}

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
  void SetUp() override {
    savedLevel_ = Logger::level();
    Logger::setOutput(LogOutput{});
  }
  void TearDown() override {
    Logger::setLevel(savedLevel_);
    Logger::setOutput(LogOutput{});
  }

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

std::string plainLinePattern(const std::string& label,
                             const std::string& module,
                             const std::string& body) {
  return R"(\[\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d{3}Z\] \[)" + label +
         R"(\] \[)" + module + R"(\] - )" + body;
}

std::string standardErrorLinePattern(const std::string& label,
                                     const std::string& module,
                                     const std::string& body) {
  return R"((\x1b\[\d+m)?)" + plainLinePattern(label, module, body) +
         R"((\x1b\[0m)?)";
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
 * Given every level name and alias that parseLevel documents
 * When each one is parsed
 * Then it yields the level it names
 */
TEST_F(LoggerTest, ParseLevelRecognizesEveryDocumentedName) {
  const auto documentedNames =
      std::to_array<std::pair<std::string_view, LogLevel>>({
          {"trace", LogLevel::Trace},
          {"debug", LogLevel::Debug},
          {"info", LogLevel::Info},
          {"warn", LogLevel::Warn},
          {"warning", LogLevel::Warn},
          {"error", LogLevel::Error},
          {"silent", LogLevel::Silent},
          {"off", LogLevel::Silent},
          {"none", LogLevel::Silent},
      });

  for (const auto& [name, level] : documentedNames) {
    EXPECT_EQ(Logger::parseLevel(name), level) << name;
  }
}

/**
 * Given level names written in upper and mixed case
 * When they are parsed
 * Then they yield the same levels as their lowercase spelling
 */
TEST_F(LoggerTest, ParseLevelIgnoresCase) {
  EXPECT_EQ(Logger::parseLevel("DEBUG"), LogLevel::Debug);
  EXPECT_EQ(Logger::parseLevel("Warning"), LogLevel::Warn);
  EXPECT_EQ(Logger::parseLevel("OFF"), LogLevel::Silent);
}

/**
 * Given a name that is not a level, and an empty name
 * When they are parsed
 * Then neither yields a level
 */
TEST_F(LoggerTest, ParseLevelRejectsUnknownNames) {
  EXPECT_FALSE(Logger::parseLevel("verbose").has_value());
  EXPECT_FALSE(Logger::parseLevel("").has_value());
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

/**
 * Given a process whose time zone is nine hours ahead of UTC
 * When a line is logged
 * Then its timestamp is the UTC time
 */
TEST_F(LoggerTest, TimestampIsInUniversalTime) {
  const TimezoneOverride tokyo("JST-9");
  Logger::setLevel(LogLevel::Info);
  const JournalFile journal;
  Logger::setOutput({.toStderr = false, .filePath = journal.path()});
  const Logger logger("Clock");

  const std::string before = universalHourPrefix();
  logger.info("tick");
  const std::string after = universalHourPrefix();

  const std::vector<std::string> lines = journal.lines();
  ASSERT_EQ(lines.size(), 1U);
  const std::string stamped = lines[0].substr(1, before.size());
  EXPECT_TRUE(stamped == before || stamped == after) << stamped;
}

/**
 * Given a file as the only sink
 * When a line is logged
 * Then the file holds it and standard error stays empty
 */
TEST_F(LoggerTest, FileSinkReceivesTheLineAndStderrStaysEmpty) {
  Logger::setLevel(LogLevel::Info);
  const JournalFile journal;
  const CerrCapture capture;
  Logger::setOutput({.toStderr = false, .filePath = journal.path()});
  const Logger logger("File");

  logger.info("hello");

  const std::vector<std::string> lines = journal.lines();
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_TRUE(std::regex_match(
      lines[0], std::regex(plainLinePattern("INFO ", "File", "hello"))));
  EXPECT_TRUE(capture.str().empty());
}

/**
 * Given a file and standard error as sinks
 * When a line is logged
 * Then standard error carries the text that the file holds
 */
TEST_F(LoggerTest, BothSinksReceiveTheSameText) {
  Logger::setLevel(LogLevel::Info);
  const JournalFile journal;
  const CerrCapture capture;
  Logger::setOutput({.toStderr = true, .filePath = journal.path()});
  const Logger logger("Both");

  logger.info("hello");

  const std::vector<std::string> lines = journal.lines();
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_NE(capture.str().find(lines[0]), std::string::npos);
}

/**
 * Given neither a file nor standard error
 * When a line is logged
 * Then nothing is written anywhere
 */
TEST_F(LoggerTest, NoSinkWritesNothing) {
  Logger::setLevel(LogLevel::Info);
  const CerrCapture capture;
  Logger::setOutput({.toStderr = false, .filePath = std::nullopt});
  const Logger logger("Nowhere");

  logger.info("lost");

  EXPECT_TRUE(capture.str().empty());
}

/**
 * Given a journal that already holds a line
 * When the output is set again on the same file
 * Then the old line is kept and the new one is appended
 */
TEST_F(LoggerTest, SettingTheSameFileAgainAppends) {
  Logger::setLevel(LogLevel::Info);
  const JournalFile journal;
  Logger::setOutput({.toStderr = false, .filePath = journal.path()});
  const Logger logger("Journal");
  logger.info("first");

  Logger::setOutput({.toStderr = false, .filePath = journal.path()});
  logger.info("second");

  const std::vector<std::string> lines = journal.lines();
  ASSERT_EQ(lines.size(), 2U);
  EXPECT_TRUE(std::regex_match(
      lines[0], std::regex(plainLinePattern("INFO ", "Journal", "first"))));
  EXPECT_TRUE(std::regex_match(
      lines[1], std::regex(plainLinePattern("INFO ", "Journal", "second"))));
}

/**
 * Given a working file sink
 * When the output is set to a file in a missing directory
 * Then the error is raised and lines still go to the previous file
 */
TEST_F(LoggerTest, UnopenableFileKeepsThePreviousOutput) {
  Logger::setLevel(LogLevel::Info);
  const JournalFile journal;
  Logger::setOutput({.toStderr = false, .filePath = journal.path()});
  const Logger logger("Config");
  const std::filesystem::path missing = std::filesystem::temp_directory_path() /
                                        "rtype_missing_directory" /
                                        "journal.log";

  EXPECT_THROW(Logger::setOutput({.toStderr = false, .filePath = missing}),
               LogFileOpenException);

  logger.info("still written");
  EXPECT_EQ(journal.lines().size(), 1U);
}

/**
 * Given a file sink and a scoped timer
 * When the scope ends
 * Then the timer line goes to the file like any other line
 */
TEST_F(LoggerTest, ScopedTimerWritesToTheFileSink) {
  Logger::setLevel(LogLevel::Info);
  const JournalFile journal;
  Logger::setOutput({.toStderr = false, .filePath = journal.path()});
  const Logger logger("Timer");
  {
    auto timer = logger.scope("phase");
  }

  const std::vector<std::string> lines = journal.lines();
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_TRUE(std::regex_match(
      lines[0], std::regex(plainLinePattern("INFO ", "Timer",
                                            R"(phase took \d+\.\d{3} ms)"))));
}
