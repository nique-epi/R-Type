#include <gtest/gtest.h>
#include <array>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <string_view>
#include <vector>
#include "JournalFile.hpp"
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"
#include "LoggingException.hpp"

using rtype::logging::InvalidLogOptionException;
using rtype::logging::LogFileOpenException;
using rtype::logging::Logger;
using rtype::logging::LogLaunchOptions;
using rtype::logging::LogLevel;
using rtype::logging::LogOutput;
using rtype::logging::MissingLogOptionValueException;
using rtype::logging::UnknownLogLevelException;

namespace {

using Arguments = std::vector<std::string_view>;

class LogLaunchOptionsTest : public ::testing::Test {
 protected:
  void SetUp() override {
    savedLevel_ = Logger::level();
    Logger::setLevel(LogLevel::Warn);
    Logger::setOutput(LogOutput{});
    originalBuffer_ = std::cerr.rdbuf(standardError_.rdbuf());
  }

  void TearDown() override {
    std::cerr.rdbuf(originalBuffer_);
    Logger::setLevel(savedLevel_);
    Logger::setOutput(LogOutput{});
  }

  [[nodiscard]] std::string standardError() const {
    return standardError_.str();
  }

 private:
  LogLevel savedLevel_{LogLevel::Info};
  std::ostringstream standardError_;
  std::streambuf* originalBuffer_{nullptr};
};

std::filesystem::path missingJournalPath() {
  return std::filesystem::temp_directory_path() / "rtype_missing_directory" /
         "journal.log";
}

}  // namespace

/**
 * Given no option and a default journal
 * When the options are applied and a warning is logged
 * Then the level is untouched, the journal holds the line and stderr is empty
 */
TEST_F(LogLaunchOptionsTest, DefaultsWriteToTheDefaultJournalOnly) {
  const JournalFile journal;

  LogLaunchOptions::read(Arguments{}, journal.path().string()).apply();
  Logger("Launch").warn("hello");

  EXPECT_EQ(Logger::level(), LogLevel::Warn);
  EXPECT_EQ(journal.lines().size(), 1U);
  EXPECT_TRUE(standardError().empty());
}

/**
 * Given the level option
 * When the options are applied
 * Then the logger has the level given by the option
 */
TEST_F(LogLaunchOptionsTest, LevelOptionSetsTheLevel) {
  const JournalFile journal;

  LogLaunchOptions::read(Arguments{"--log-level", "debug"},
                         journal.path().string())
      .apply();

  EXPECT_EQ(Logger::level(), LogLevel::Debug);
}

/**
 * Given the option that removes the file
 * When the options are applied and a line is logged
 * Then no journal is created and stderr stays empty
 */
TEST_F(LogLaunchOptionsTest, NoFileOptionWritesNothing) {
  const JournalFile journal;

  LogLaunchOptions::read(Arguments{"--no-log-file"}, journal.path().string())
      .apply();
  Logger("Launch").warn("lost");

  EXPECT_FALSE(std::filesystem::exists(journal.path()));
  EXPECT_TRUE(standardError().empty());
}

/**
 * Given the options that remove the file and enable stderr
 * When a line is logged
 * Then stderr holds it and no journal exists
 */
TEST_F(LogLaunchOptionsTest, StderrOptionWritesToStandardError) {
  const JournalFile journal;

  LogLaunchOptions::read(Arguments{"--no-log-file", "--log-stderr"},
                         journal.path().string())
      .apply();
  Logger("Launch").warn("shown");

  EXPECT_NE(standardError().find("shown"), std::string::npos);
  EXPECT_FALSE(std::filesystem::exists(journal.path()));
}

/**
 * Given the file removed and then given again
 * When a line is logged
 * Then the last option wins and the line goes to the file
 */
TEST_F(LogLaunchOptionsTest, FileOptionAfterNoFileOptionWritesToTheFile) {
  const JournalFile journal;
  const std::string path = journal.path().string();

  LogLaunchOptions::read(Arguments{"--no-log-file", "--log-file", path},
                         "unused.log")
      .apply();
  Logger("Launch").warn("kept");

  EXPECT_EQ(journal.lines().size(), 1U);
}

/**
 * Given the file given and then removed
 * When a line is logged
 * Then the last option wins and no journal is created
 */
TEST_F(LogLaunchOptionsTest, NoFileOptionAfterFileOptionWritesNothing) {
  const JournalFile journal;
  const std::string path = journal.path().string();

  LogLaunchOptions::read(Arguments{"--log-file", path, "--no-log-file"},
                         "unused.log")
      .apply();
  Logger("Launch").warn("lost");

  EXPECT_FALSE(std::filesystem::exists(journal.path()));
}

/**
 * Given a level option that is no level
 * When the options are read
 * Then it is rejected and neither the level nor the output changed
 */
TEST_F(LogLaunchOptionsTest, UnknownLevelIsRejectedAndChangesNothing) {
  const JournalFile journal;
  ASSERT_EQ(Logger::level(), LogLevel::Warn);

  EXPECT_THROW(
      static_cast<void>(LogLaunchOptions::read(
          Arguments{"--log-level", "verbose"}, journal.path().string())),
      UnknownLogLevelException);

  EXPECT_EQ(Logger::level(), LogLevel::Warn);
  Logger("Launch").warn("still on stderr");
  EXPECT_NE(standardError().find("still on stderr"), std::string::npos);
  EXPECT_FALSE(std::filesystem::exists(journal.path()));
}

/**
 * Given a good level and a journal that cannot be opened
 * When the options are applied
 * Then the error is raised and neither the level nor the output changed
 */
TEST_F(LogLaunchOptionsTest, UnopenableJournalChangesNothing) {
  const std::string missing = missingJournalPath().string();
  const LogLaunchOptions options = LogLaunchOptions::read(
      Arguments{"--log-level", "debug", "--log-file", missing}, "unused.log");
  ASSERT_EQ(Logger::level(), LogLevel::Warn);

  EXPECT_THROW(options.apply(), LogFileOpenException);

  EXPECT_EQ(Logger::level(), LogLevel::Warn);
  Logger("Launch").warn("still on stderr");
  EXPECT_NE(standardError().find("still on stderr"), std::string::npos);
}

/**
 * Given an option that needs a value as the last argument
 * When the options are read
 * Then the missing value is reported
 */
TEST_F(LogLaunchOptionsTest, OptionWithoutValueIsRejected) {
  EXPECT_THROW(static_cast<void>(LogLaunchOptions::read(
                   Arguments{"--log-level"}, "unused.log")),
               MissingLogOptionValueException);
  EXPECT_THROW(static_cast<void>(LogLaunchOptions::read(Arguments{"--log-file"},
                                                        "unused.log")),
               MissingLogOptionValueException);
}

/**
 * Given a logging option that does not exist, under either prefix
 * When the options are read
 * Then it is rejected
 */
TEST_F(LogLaunchOptionsTest, UnknownLoggingOptionIsRejected) {
  EXPECT_THROW(static_cast<void>(LogLaunchOptions::read(
                   Arguments{"--log-levle", "debug"}, "unused.log")),
               InvalidLogOptionException);
  EXPECT_THROW(static_cast<void>(LogLaunchOptions::read(
                   Arguments{"--no-log-stderr"}, "unused.log")),
               InvalidLogOptionException);
}

/**
 * Given a level and a journal asked for
 * When the options are only read
 * Then the level is untouched and no journal is created
 */
TEST_F(LogLaunchOptionsTest, ReadingAloneChangesNothing) {
  const JournalFile journal;
  const std::string path = journal.path().string();

  static_cast<void>(LogLaunchOptions::read(
      Arguments{"--log-level", "error", "--log-file", path}, "unused.log"));

  EXPECT_EQ(Logger::level(), LogLevel::Warn);
  EXPECT_FALSE(std::filesystem::exists(journal.path()));
}

/**
 * Given logging options mixed with arguments of the program
 * When the options are read
 * Then the program's arguments are kept in order, without the logging
 *      options or their values
 */
TEST_F(LogLaunchOptionsTest, OtherArgumentsAreKeptInOrder) {
  const LogLaunchOptions options =
      LogLaunchOptions::read(Arguments{"-p", "4243", "--log-level", "debug",
                                       "-h", "127.0.0.1", "--log-stderr"},
                             "unused.log");

  const Arguments otherArguments(options.otherArguments().begin(),
                                 options.otherArguments().end());
  EXPECT_EQ(otherArguments, (Arguments{"-p", "4243", "-h", "127.0.0.1"}));
}

/**
 * Given arguments that have nothing to do with logging
 * When the options are read and applied
 * Then the defaults apply
 */
TEST_F(LogLaunchOptionsTest, OtherArgumentsLeaveTheDefaults) {
  const JournalFile journal;

  LogLaunchOptions::read(Arguments{"--port", "4242", "-v"},
                         journal.path().string())
      .apply();
  Logger("Launch").warn("hello");

  EXPECT_EQ(Logger::level(), LogLevel::Warn);
  EXPECT_EQ(journal.lines().size(), 1U);
}

/**
 * Given the process arguments with the program name first
 * When the options are read from the process and applied
 * Then the options after the program name apply
 */
TEST_F(LogLaunchOptionsTest, ProcessArgumentsApplyAfterTheProgramName) {
  const JournalFile journal;
  const std::array<const char*, 3> arguments{"r-type_server", "--log-level",
                                             "error"};

  LogLaunchOptions::readFromProcess(static_cast<int>(arguments.size()),
                                    arguments.data(), journal.path().string())
      .apply();

  EXPECT_EQ(Logger::level(), LogLevel::Error);
}

/**
 * Given the process arguments with the program name first
 * When the options are read from the process
 * Then the program name is not among the other arguments
 */
TEST_F(LogLaunchOptionsTest, ProcessArgumentsLeaveTheProgramNameOut) {
  const std::array<const char*, 3> arguments{"r-type_server", "-p", "4243"};

  const LogLaunchOptions options = LogLaunchOptions::readFromProcess(
      static_cast<int>(arguments.size()), arguments.data(), "unused.log");

  const Arguments otherArguments(options.otherArguments().begin(),
                                 options.otherArguments().end());
  EXPECT_EQ(otherArguments, (Arguments{"-p", "4243"}));
}
