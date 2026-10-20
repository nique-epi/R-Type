#include <gtest/gtest.h>
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>
#include "JournalFile.hpp"
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"
#include "LoggingConstants.hpp"
#include "LoggingException.hpp"

using rtype::logging::FILE_OPTION;
using rtype::logging::InvalidLogOptionException;
using rtype::logging::LEVEL_OPTION;
using rtype::logging::LogFileOpenException;
using rtype::logging::Logger;
using rtype::logging::LogLaunchOptions;
using rtype::logging::LogLevel;
using rtype::logging::LogOutput;
using rtype::logging::MissingLogOptionValueException;
using rtype::logging::NO_FILE_OPTION;
using rtype::logging::NO_STDERR_OPTION;
using rtype::logging::STDERR_OPTION;
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

/** @brief Makes a directory the current one until it is destroyed. */
class CurrentDirectoryChange {
 public:
  explicit CurrentDirectoryChange(const std::filesystem::path& directory)
      : previous_(std::filesystem::current_path()) {
    std::filesystem::current_path(directory);
  }

  ~CurrentDirectoryChange() {
    std::error_code error;
    std::filesystem::current_path(previous_, error);
  }

  CurrentDirectoryChange(const CurrentDirectoryChange&) = delete;
  CurrentDirectoryChange& operator=(const CurrentDirectoryChange&) = delete;
  CurrentDirectoryChange(CurrentDirectoryChange&&) = delete;
  CurrentDirectoryChange& operator=(CurrentDirectoryChange&&) = delete;

 private:
  std::filesystem::path previous_;
};

}  // namespace

/**
 * Given no option and a default journal
 * When the options are applied and a warning is logged
 * Then the level is untouched, and the line is in the journal and on stderr
 */
TEST_F(LogLaunchOptionsTest, DefaultsWriteToTheJournalAndStandardError) {
  const JournalFile journal;

  LogLaunchOptions::read(Arguments{}, journal.path().string()).apply();
  Logger("Launch").warn("hello");

  EXPECT_EQ(Logger::level(), LogLevel::Warn);
  EXPECT_EQ(journal.lines().size(), 1U);
  EXPECT_NE(standardError().find("hello"), std::string::npos);
}

/**
 * Given the option that turns standard error off
 * When the options are applied and a warning is logged
 * Then the journal holds the line and stderr stays empty
 */
TEST_F(LogLaunchOptionsTest, NoStderrOptionWritesToTheJournalOnly) {
  const JournalFile journal;

  LogLaunchOptions::read(Arguments{"--no-log-stderr"}, journal.path().string())
      .apply();
  Logger("Launch").warn("hello");

  EXPECT_EQ(journal.lines().size(), 1U);
  EXPECT_TRUE(standardError().empty());
}

/**
 * Given standard error turned off, then on again
 * When a line is logged
 * Then the last option wins and stderr holds the line
 */
TEST_F(LogLaunchOptionsTest, StderrOptionAfterNoStderrOptionWritesToStderr) {
  LogLaunchOptions::read(
      Arguments{"--no-log-file", "--no-log-stderr", "--log-stderr"},
      "unused.log")
      .apply();
  Logger("Launch").warn("shown");

  EXPECT_NE(standardError().find("shown"), std::string::npos);
}

/**
 * Given standard error turned on, then off
 * When a line is logged
 * Then the last option wins and stderr stays empty
 */
TEST_F(LogLaunchOptionsTest, NoStderrOptionAfterStderrOptionKeepsStderrEmpty) {
  const JournalFile journal;

  LogLaunchOptions::read(Arguments{"--log-stderr", "--no-log-stderr"},
                         journal.path().string())
      .apply();
  Logger("Launch").warn("hidden");

  EXPECT_TRUE(standardError().empty());
}

/**
 * Given a journal given by its absolute path and the info level
 * When the options are applied
 * Then the first line of the journal names that path
 */
TEST_F(LogLaunchOptionsTest, FirstLineNamesTheJournal) {
  const JournalFile journal;

  LogLaunchOptions::read(
      Arguments{"--log-level", "info", "--log-file", journal.path().string()},
      "unused.log")
      .apply();

  const std::vector<std::string> lines = journal.lines();
  ASSERT_FALSE(lines.empty());
  EXPECT_NE(lines.front().find("journal: " + journal.path().string()),
            std::string::npos);
}

/**
 * Given a journal given by a path relative to the current directory
 * When the options are applied at the info level
 * Then the first line of the journal names it by its absolute path
 */
TEST_F(LogLaunchOptionsTest, FirstLineNamesARelativeJournalByItsAbsolutePath) {
  const CurrentDirectoryChange directory{
      std::filesystem::temp_directory_path()};
  const std::filesystem::path relativeJournal{
      "LogLaunchOptionsTest_relative_journal.log"};
  const std::filesystem::path absoluteJournal =
      std::filesystem::current_path() / relativeJournal;
  std::filesystem::remove(absoluteJournal);

  LogLaunchOptions::read(Arguments{"--log-level", "info"},
                         relativeJournal.string())
      .apply();
  Logger::setOutput(LogOutput{});

  std::ifstream journal{absoluteJournal};
  std::string firstLine;
  std::getline(journal, firstLine);
  journal.close();
  std::filesystem::remove(absoluteJournal);
  EXPECT_NE(firstLine.find("journal: " + absoluteJournal.string()),
            std::string::npos);
}

/**
 * Given the option that removes the journal and the info level
 * When the options are applied
 * Then stderr says that there is no journal file
 */
TEST_F(LogLaunchOptionsTest, AnnouncesThatThereIsNoJournal) {
  LogLaunchOptions::read(Arguments{"--log-level", "info", "--no-log-file"},
                         "unused.log")
      .apply();

  EXPECT_NE(standardError().find("no journal file"), std::string::npos);
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
 * Then no journal is created and stderr holds the line
 */
TEST_F(LogLaunchOptionsTest, NoFileOptionWritesToStandardErrorOnly) {
  const JournalFile journal;

  LogLaunchOptions::read(Arguments{"--no-log-file"}, journal.path().string())
      .apply();
  Logger("Launch").warn("shown");

  EXPECT_FALSE(std::filesystem::exists(journal.path()));
  EXPECT_NE(standardError().find("shown"), std::string::npos);
}

/**
 * Given the options that remove the file and standard error
 * When the options are applied and a line is logged
 * Then no journal is created and stderr stays empty
 */
TEST_F(LogLaunchOptionsTest, NoFileAndNoStderrOptionsWriteNothing) {
  const JournalFile journal;

  LogLaunchOptions::read(Arguments{"--no-log-file", "--no-log-stderr"},
                         journal.path().string())
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
                   Arguments{"--no-log-level"}, "unused.log")),
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

/**
 * Given the logging options
 * When their usage is built
 * Then each option starts a line of it
 */
TEST(LogLaunchOptionsUsage, NamesEveryLoggingOption) {
  const std::string usage = LogLaunchOptions::usage("unused.log");

  for (const std::string_view option :
       {LEVEL_OPTION, FILE_OPTION, NO_FILE_OPTION, STDERR_OPTION,
        NO_STDERR_OPTION}) {
    SCOPED_TRACE(option);
    EXPECT_NE(usage.find("\n  " + std::string(option)), std::string::npos);
  }
}

/**
 * Given a default journal
 * When the usage of the logging options is built
 * Then it names that journal
 */
TEST(LogLaunchOptionsUsage, NamesTheDefaultJournal) {
  const std::string usage = LogLaunchOptions::usage("r-type_server.log");

  EXPECT_NE(usage.find("Default: r-type_server.log."), std::string::npos);
}
