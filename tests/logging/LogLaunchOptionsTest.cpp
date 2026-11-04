#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include "CurrentDirectoryChange.hpp"
#include "JournalFile.hpp"
#include "LaunchOptionReadingTestConstants.hpp"
#include "LogLaunchOptions.hpp"
#include "LogLaunchOptionsFixture.hpp"
#include "LogLaunchOptionsFixtures.hpp"
#include "LogLaunchOptionsTestConstants.hpp"
#include "Logger.hpp"
#include "LoggingConstants.hpp"
#include "LoggingException.hpp"
#include "LoggingOptionTestConstants.hpp"
#include "NonAnsiFolderTestConstants.hpp"
#include "TemporaryFolder.hpp"

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
using rtype::logging::SERVER_LOG_FILE_NAME;
using rtype::logging::STDERR_OPTION;
using rtype::logging::UnknownLogLevelException;
using rtype::logging::testing::DEFAULT_SERVER_JOURNAL_TEXT;
using rtype::logging::testing::ERROR_LEVEL_ARGUMENT;
using rtype::logging::testing::HELLO_LINE;
using rtype::logging::testing::HIDDEN_LINE;
using rtype::logging::testing::INFO_LEVEL_ARGUMENT;
using rtype::logging::testing::JOURNAL_LINE_START;
using rtype::logging::testing::KEPT_LINE;
using rtype::logging::testing::LAUNCH_LOGGER_NAME;
using rtype::logging::testing::LOST_LINE;
using rtype::logging::testing::MISSING_JOURNAL;
using rtype::logging::testing::NO_JOURNAL_LINE;
using rtype::logging::testing::NO_LEVEL_OPTION;
using rtype::logging::testing::NOT_A_LEVEL_ARGUMENT;
using rtype::logging::testing::OPTION_LINE_START;
using rtype::logging::testing::SHOWN_LINE;
using rtype::logging::testing::STILL_ON_STDERR_LINE;
using rtype::logging::testing::UNOPENED_JOURNAL_LINE_START;
using rtype::logging::testing::UNUSED_JOURNAL;
using rtype::testing::NON_ANSI_FOLDER_BYTES;
using rtype::testing::NON_ANSI_FOLDER_NAME;

using Arguments = std::vector<std::string_view>;

/**
 * Given no option and a default journal
 * When the options are applied and a warning is logged
 * Then the level is untouched, and the line is in the journal and on stderr
 */
TEST_F(LogLaunchOptionsFixture, DefaultsWriteToTheJournalAndStandardError) {
  const JournalFile journal;

  logOptionsOf(Arguments{}, journal.path()).apply();
  Logger(std::string(LAUNCH_LOGGER_NAME)).warn(HELLO_LINE);

  EXPECT_EQ(Logger::level(), LogLevel::Warn);
  EXPECT_EQ(journal.lines().size(), 1U);
  EXPECT_NE(standardError().find(HELLO_LINE), std::string::npos);
}

/**
 * Given the option that turns standard error off
 * When the options are applied and a warning is logged
 * Then the journal holds the line and stderr stays empty
 */
TEST_F(LogLaunchOptionsFixture, NoStderrOptionWritesToTheJournalOnly) {
  const JournalFile journal;

  logOptionsOf(Arguments{NO_STDERR_OPTION}, journal.path()).apply();
  Logger(std::string(LAUNCH_LOGGER_NAME)).warn(HELLO_LINE);

  EXPECT_EQ(journal.lines().size(), 1U);
  EXPECT_TRUE(standardError().empty());
}

/**
 * Given standard error turned off, then on again
 * When a line is logged
 * Then the last option wins and stderr holds the line
 */
TEST_F(LogLaunchOptionsFixture, StderrOptionAfterNoStderrOptionWritesToStderr) {
  logOptionsOf(Arguments{NO_FILE_OPTION, NO_STDERR_OPTION, STDERR_OPTION},
               UNUSED_JOURNAL)
      .apply();
  Logger(std::string(LAUNCH_LOGGER_NAME)).warn(SHOWN_LINE);

  EXPECT_NE(standardError().find(SHOWN_LINE), std::string::npos);
}

/**
 * Given standard error turned on, then off
 * When a line is logged
 * Then the last option wins and stderr stays empty
 */
TEST_F(LogLaunchOptionsFixture,
       NoStderrOptionAfterStderrOptionKeepsStderrEmpty) {
  const JournalFile journal;

  logOptionsOf(Arguments{STDERR_OPTION, NO_STDERR_OPTION}, journal.path())
      .apply();
  Logger(std::string(LAUNCH_LOGGER_NAME)).warn(HIDDEN_LINE);

  EXPECT_TRUE(standardError().empty());
}

/**
 * Given a journal given by its absolute path and the info level
 * When the options are applied
 * Then the first line of the journal names that path
 */
TEST_F(LogLaunchOptionsFixture, FirstLineNamesTheJournal) {
  const JournalFile journal;

  logOptionsOf(Arguments{LEVEL_OPTION, INFO_LEVEL_ARGUMENT, FILE_OPTION,
                         journal.path().string()},
               UNUSED_JOURNAL)
      .apply();

  const std::vector<std::string> lines = journal.lines();
  ASSERT_FALSE(lines.empty());
  EXPECT_NE(lines.front().find(std::string(JOURNAL_LINE_START) +
                               journal.path().string()),
            std::string::npos);
}

/**
 * Given a default journal in a folder whose name is outside the code page
 * When the options are applied at the info level
 * Then the first line of the journal names it in UTF-8, and nothing is thrown
 */
TEST_F(LogLaunchOptionsFixture, FirstLineNamesAJournalInANonAnsiFolder) {
  const TemporaryFolder folder;
  const std::filesystem::path journalFolder =
      folder.path() / NON_ANSI_FOLDER_NAME;
  std::filesystem::create_directories(journalFolder);
  const std::filesystem::path journal = journalFolder / "journal.log";

  logOptionsOf(Arguments{LEVEL_OPTION, INFO_LEVEL_ARGUMENT}, journal).apply();
  Logger::setOutput(LogOutput{});

  std::ifstream written(journal);
  std::string firstLine;
  std::getline(written, firstLine);
  EXPECT_NE(firstLine.find(std::string{NON_ANSI_FOLDER_BYTES} + "/journal.log"),
            std::string::npos);
}

/**
 * Given a journal given by a path relative to the current directory
 * When the options are applied at the info level
 * Then the first line of the journal names it by its absolute path
 */
TEST_F(LogLaunchOptionsFixture,
       FirstLineNamesARelativeJournalByItsAbsolutePath) {
  const JournalFile journal;
  const CurrentDirectoryChange directory{journal.path().parent_path()};
  const std::filesystem::path relativeJournal = journal.path().filename();

  logOptionsOf(Arguments{LEVEL_OPTION, INFO_LEVEL_ARGUMENT}, relativeJournal)
      .apply();

  const std::vector<std::string> lines = journal.lines();
  ASSERT_FALSE(lines.empty());
  const std::filesystem::path absoluteJournal =
      std::filesystem::current_path() / relativeJournal;
  EXPECT_NE(lines.front().find(std::string(JOURNAL_LINE_START) +
                               absoluteJournal.string()),
            std::string::npos);
}

/**
 * Given the option that removes the journal and the info level
 * When the options are applied
 * Then stderr says that there is no journal file
 */
TEST_F(LogLaunchOptionsFixture, AnnouncesThatThereIsNoJournal) {
  logOptionsOf(Arguments{LEVEL_OPTION, INFO_LEVEL_ARGUMENT, NO_FILE_OPTION},
               UNUSED_JOURNAL)
      .apply();

  EXPECT_NE(standardError().find(NO_JOURNAL_LINE), std::string::npos);
}

/**
 * Given the level option
 * When the options are applied
 * Then the logger has the level given by the option
 */
TEST_F(LogLaunchOptionsFixture, LevelOptionSetsTheLevel) {
  const JournalFile journal;

  logOptionsOf(Arguments{LEVEL_OPTION, DEBUG_LEVEL_ARGUMENT}, journal.path())
      .apply();

  EXPECT_EQ(Logger::level(), LogLevel::Debug);
}

/**
 * Given the option that removes the file
 * When the options are applied and a line is logged
 * Then no journal is created and stderr holds the line
 */
TEST_F(LogLaunchOptionsFixture, NoFileOptionWritesToStandardErrorOnly) {
  const JournalFile journal;

  logOptionsOf(Arguments{NO_FILE_OPTION}, journal.path()).apply();
  Logger(std::string(LAUNCH_LOGGER_NAME)).warn(SHOWN_LINE);

  EXPECT_FALSE(std::filesystem::exists(journal.path()));
  EXPECT_NE(standardError().find(SHOWN_LINE), std::string::npos);
}

/**
 * Given the options that remove the file and standard error
 * When the options are applied and a line is logged
 * Then no journal is created and stderr stays empty
 */
TEST_F(LogLaunchOptionsFixture, NoFileAndNoStderrOptionsWriteNothing) {
  const JournalFile journal;

  logOptionsOf(Arguments{NO_FILE_OPTION, NO_STDERR_OPTION}, journal.path())
      .apply();
  Logger(std::string(LAUNCH_LOGGER_NAME)).warn(LOST_LINE);

  EXPECT_FALSE(std::filesystem::exists(journal.path()));
  EXPECT_TRUE(standardError().empty());
}

/**
 * Given the options that remove the file and enable stderr
 * When a line is logged
 * Then stderr holds it and no journal exists
 */
TEST_F(LogLaunchOptionsFixture, StderrOptionWritesToStandardError) {
  const JournalFile journal;

  logOptionsOf(Arguments{NO_FILE_OPTION, STDERR_OPTION}, journal.path())
      .apply();
  Logger(std::string(LAUNCH_LOGGER_NAME)).warn(SHOWN_LINE);

  EXPECT_NE(standardError().find(SHOWN_LINE), std::string::npos);
  EXPECT_FALSE(std::filesystem::exists(journal.path()));
}

/**
 * Given the file removed and then given again
 * When a line is logged
 * Then the last option wins and the line goes to the file
 */
TEST_F(LogLaunchOptionsFixture, FileOptionAfterNoFileOptionWritesToTheFile) {
  const JournalFile journal;
  const std::string path = journal.path().string();

  logOptionsOf(Arguments{NO_FILE_OPTION, FILE_OPTION, path}, UNUSED_JOURNAL)
      .apply();
  Logger(std::string(LAUNCH_LOGGER_NAME)).warn(KEPT_LINE);

  EXPECT_EQ(journal.lines().size(), 1U);
}

/**
 * Given the file given and then removed
 * When a line is logged
 * Then the last option wins and no journal is created
 */
TEST_F(LogLaunchOptionsFixture, NoFileOptionAfterFileOptionWritesNothing) {
  const JournalFile journal;
  const std::string path = journal.path().string();

  logOptionsOf(Arguments{FILE_OPTION, path, NO_FILE_OPTION}, UNUSED_JOURNAL)
      .apply();
  Logger(std::string(LAUNCH_LOGGER_NAME)).warn(LOST_LINE);

  EXPECT_FALSE(std::filesystem::exists(journal.path()));
}

/**
 * Given a level option that is no level
 * When the options are read
 * Then it is rejected and neither the level nor the output changed
 */
TEST_F(LogLaunchOptionsFixture, UnknownLevelIsRejectedAndChangesNothing) {
  const JournalFile journal;
  ASSERT_EQ(Logger::level(), LogLevel::Warn);

  EXPECT_THROW(
      static_cast<void>(logOptionsOf(
          Arguments{LEVEL_OPTION, NOT_A_LEVEL_ARGUMENT}, journal.path())),
      UnknownLogLevelException);

  EXPECT_EQ(Logger::level(), LogLevel::Warn);
  Logger(std::string(LAUNCH_LOGGER_NAME)).warn(STILL_ON_STDERR_LINE);
  EXPECT_NE(standardError().find(STILL_ON_STDERR_LINE), std::string::npos);
  EXPECT_FALSE(std::filesystem::exists(journal.path()));
}

/**
 * Given a good level and a journal that cannot be opened
 * When the options are applied
 * Then the error is raised and neither the level nor the output changed
 */
TEST_F(LogLaunchOptionsFixture, UnopenableJournalChangesNothing) {
  const std::string missing =
      (std::filesystem::temp_directory_path() / MISSING_JOURNAL).string();
  const LogLaunchOptions options = logOptionsOf(
      Arguments{LEVEL_OPTION, DEBUG_LEVEL_ARGUMENT, FILE_OPTION, missing},
      UNUSED_JOURNAL);
  ASSERT_EQ(Logger::level(), LogLevel::Warn);

  EXPECT_THROW(options.apply(), LogFileOpenException);

  EXPECT_EQ(Logger::level(), LogLevel::Warn);
  Logger(std::string(LAUNCH_LOGGER_NAME)).warn(STILL_ON_STDERR_LINE);
  EXPECT_NE(standardError().find(STILL_ON_STDERR_LINE), std::string::npos);
}

/**
 * Given a level and a default journal in a folder that does not exist
 * When the options are applied
 * Then nothing is raised, the level is set, no file is created, and standard
 * error names the absolute path of the journal it could not open, as Windows
 * writes it with its own separators, and says there is none
 */
TEST_F(LogLaunchOptionsFixture,
       UnopenableDefaultJournalWritesToStandardErrorOnly) {
  const std::filesystem::path missing =
      std::filesystem::temp_directory_path() / MISSING_JOURNAL;
  const LogLaunchOptions options =
      logOptionsOf(Arguments{LEVEL_OPTION, DEBUG_LEVEL_ARGUMENT}, missing);

  EXPECT_NO_THROW(options.apply());

  EXPECT_EQ(Logger::level(), LogLevel::Debug);
  EXPECT_FALSE(std::filesystem::exists(missing));
  EXPECT_NE(standardError().find(std::string(UNOPENED_JOURNAL_LINE_START) +
                                 std::filesystem::absolute(missing).string()),
            std::string::npos);
  EXPECT_NE(standardError().find(NO_JOURNAL_LINE), std::string::npos);
}

/**
 * Given `--no-log-stderr` and a default journal that cannot be opened
 * When the options are applied and a warning is logged
 * Then nothing is raised and nothing reaches standard error, as asked
 */
TEST_F(LogLaunchOptionsFixture, UnopenableDefaultJournalKeepsStandardErrorOff) {
  const std::filesystem::path missing =
      std::filesystem::temp_directory_path() / MISSING_JOURNAL;
  const LogLaunchOptions options =
      logOptionsOf(Arguments{NO_STDERR_OPTION}, missing);

  EXPECT_NO_THROW(options.apply());

  Logger(std::string(LAUNCH_LOGGER_NAME)).warn(LOST_LINE);
  EXPECT_EQ(standardError(), "");
}

/**
 * Given an option that needs a value as the last argument
 * When the options are read
 * Then the missing value is reported
 */
TEST_F(LogLaunchOptionsFixture, OptionWithoutValueIsRejected) {
  EXPECT_THROW(
      static_cast<void>(logOptionsOf(Arguments{LEVEL_OPTION}, UNUSED_JOURNAL)),
      MissingLogOptionValueException);
  EXPECT_THROW(
      static_cast<void>(logOptionsOf(Arguments{FILE_OPTION}, UNUSED_JOURNAL)),
      MissingLogOptionValueException);
}

/**
 * Given a logging option that does not exist, under either prefix
 * When the options are read
 * Then it is rejected
 */
TEST_F(LogLaunchOptionsFixture, UnknownLoggingOptionIsRejected) {
  EXPECT_THROW(static_cast<void>(logOptionsOf(
                   Arguments{MISSPELLED_LEVEL_OPTION, DEBUG_LEVEL_ARGUMENT},
                   UNUSED_JOURNAL)),
               InvalidLogOptionException);
  EXPECT_THROW(static_cast<void>(
                   logOptionsOf(Arguments{NO_LEVEL_OPTION}, UNUSED_JOURNAL)),
               InvalidLogOptionException);
}

/**
 * Given a level and a journal asked for
 * When the options are only read
 * Then the level is untouched and no journal is created
 */
TEST_F(LogLaunchOptionsFixture, ReadingAloneChangesNothing) {
  const JournalFile journal;
  const std::string path = journal.path().string();

  static_cast<void>(logOptionsOf(
      Arguments{LEVEL_OPTION, ERROR_LEVEL_ARGUMENT, FILE_OPTION, path},
      UNUSED_JOURNAL));

  EXPECT_EQ(Logger::level(), LogLevel::Warn);
  EXPECT_FALSE(std::filesystem::exists(journal.path()));
}

/**
 * Given the level option, a level, then another logging option
 * When the logging option the arguments start with is read
 * Then it takes the option and its value
 */
TEST(LogLaunchOptionsReading, TakesAnOptionAndItsValue) {
  LogLaunchOptions options{UNUSED_JOURNAL};

  EXPECT_EQ(options.readOption(
                Arguments{LEVEL_OPTION, DEBUG_LEVEL_ARGUMENT, NO_FILE_OPTION}),
            AN_OPTION_AND_ITS_VALUE);
}

/**
 * Given the option that removes the journal, then another logging option
 * When the logging option the arguments start with is read
 * Then it takes that option alone
 */
TEST(LogLaunchOptionsReading, TakesAnOptionAlone) {
  LogLaunchOptions options{UNUSED_JOURNAL};

  EXPECT_EQ(options.readOption(Arguments{NO_FILE_OPTION, LEVEL_OPTION}),
            AN_OPTION_ALONE);
}

/**
 * Given a level without the option before it
 * When the logging option the arguments start with is read
 * Then nothing is taken: it is left to the program
 */
TEST(LogLaunchOptionsReading, TakesNoArgumentThatIsNoLoggingOption) {
  LogLaunchOptions options{UNUSED_JOURNAL};

  EXPECT_EQ(options.readOption(Arguments{DEBUG_LEVEL_ARGUMENT}),
            NO_ARGUMENT_TAKEN);
}

/**
 * Given no argument
 * When the logging option the arguments start with is read
 * Then nothing is taken
 */
TEST(LogLaunchOptionsReading, TakesNothingFromNoArgument) {
  LogLaunchOptions options{UNUSED_JOURNAL};

  EXPECT_EQ(options.readOption(Arguments{}), NO_ARGUMENT_TAKEN);
}

/**
 * Given the journal option followed by another logging option
 * When the logging option the arguments start with is read
 * Then the other option is the value of the journal option
 */
TEST(LogLaunchOptionsReading, TakesTheArgumentAfterAnOptionAsItsValue) {
  LogLaunchOptions options{UNUSED_JOURNAL};

  EXPECT_EQ(options.readOption(Arguments{FILE_OPTION, NO_FILE_OPTION}),
            AN_OPTION_AND_ITS_VALUE);
}

/**
 * Given a level without the option before it, and a default journal
 * When it is handed to the logging options, then they are applied
 * Then the defaults apply
 */
TEST_F(LogLaunchOptionsFixture,
       ArgumentThatIsNoLoggingOptionLeavesTheDefaults) {
  const JournalFile journal;
  LogLaunchOptions options{journal.path()};

  static_cast<void>(options.readOption(Arguments{DEBUG_LEVEL_ARGUMENT}));
  options.apply();
  Logger(std::string(LAUNCH_LOGGER_NAME)).warn(HELLO_LINE);

  EXPECT_EQ(Logger::level(), LogLevel::Warn);
  EXPECT_EQ(journal.lines().size(), 1U);
}

/**
 * Given the logging options
 * When their usage is built
 * Then each option starts a line of it
 */
TEST(LogLaunchOptionsUsage, NamesEveryLoggingOption) {
  const std::string usage = LogLaunchOptions::usage(UNUSED_JOURNAL);

  for (const std::string_view option :
       {LEVEL_OPTION, FILE_OPTION, NO_FILE_OPTION, STDERR_OPTION,
        NO_STDERR_OPTION}) {
    SCOPED_TRACE(option);
    EXPECT_NE(usage.find(std::string(OPTION_LINE_START) + std::string(option)),
              std::string::npos);
  }
}

/**
 * Given the name of a default journal
 * When the usage of the logging options is built
 * Then it names that journal, says it sits next to the executable, and that
 * there is none when that file cannot be opened
 */
TEST(LogLaunchOptionsUsage, NamesTheDefaultJournalAndItsPlace) {
  const std::string usage = LogLaunchOptions::usage(SERVER_LOG_FILE_NAME);

  EXPECT_NE(usage.find(DEFAULT_SERVER_JOURNAL_TEXT), std::string::npos);
}
