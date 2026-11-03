#include <gtest/gtest.h>
#include <array>
#include <string>
#include <string_view>
#include <vector>
#include "EngineException.hpp"
#include "LaunchArguments.hpp"
#include "LaunchArgumentsFixtures.hpp"
#include "LaunchArgumentsTestConstants.hpp"
#include "LaunchOption.hpp"

using rtype::engine::launchErrorText;
using rtype::engine::LaunchOption;
using rtype::engine::MissingLaunchOptionValueException;
using rtype::engine::processArguments;
using rtype::engine::readLaunchArguments;
using rtype::engine::UnknownLaunchArgumentException;
using rtype::engine::usageEntry;
using rtype::engine::usageHeader;

using Arguments = std::vector<std::string_view>;

/**
 * Given no argument
 * When the arguments are read
 * Then no record is called
 */
TEST(LaunchArguments, NoArgumentRecordsNothing) {
  std::vector<RecordedCall> calls;

  readLaunchArguments(Arguments{}, recordingOptions(calls));

  EXPECT_TRUE(calls.empty());
}

/**
 * Given an option that takes a value, followed by a value
 * When the arguments are read
 * Then the option's record gets that value
 */
TEST(LaunchArguments, HandsTheNextArgumentToAnOptionThatTakesAValue) {
  std::vector<RecordedCall> calls;

  readLaunchArguments(Arguments{VALUE_OPTION, FIRST_VALUE},
                      recordingOptions(calls));

  EXPECT_EQ(calls,
            (std::vector<RecordedCall>{{.option = std::string(VALUE_OPTION),
                                        .value = std::string(FIRST_VALUE)}}));
}

/**
 * Given an option that takes no value
 * When the arguments are read
 * Then the option's record gets an empty value
 */
TEST(LaunchArguments, HandsAnEmptyValueToAnOptionWithoutValue) {
  std::vector<RecordedCall> calls;

  readLaunchArguments(Arguments{FLAG_OPTION}, recordingOptions(calls));

  EXPECT_EQ(calls, (std::vector<RecordedCall>{
                       {.option = std::string(FLAG_OPTION), .value = ""}}));
}

/**
 * Given an option given twice, around another one
 * When the arguments are read
 * Then the records are called in the order of the arguments
 */
TEST(LaunchArguments, RecordsTheOptionsInTheOrderGiven) {
  std::vector<RecordedCall> calls;

  readLaunchArguments(Arguments{VALUE_OPTION, FIRST_VALUE, FLAG_OPTION,
                                VALUE_OPTION, SECOND_VALUE},
                      recordingOptions(calls));

  EXPECT_EQ(calls, (std::vector<RecordedCall>{
                       {.option = std::string(VALUE_OPTION),
                        .value = std::string(FIRST_VALUE)},
                       {.option = std::string(FLAG_OPTION), .value = ""},
                       {.option = std::string(VALUE_OPTION),
                        .value = std::string(SECOND_VALUE)}}));
}

/**
 * Given an option that takes a value, followed by another option
 * When the arguments are read
 * Then the other option is the value, and its own record is not called
 */
TEST(LaunchArguments, ReadsTheArgumentAfterAValueOptionAsItsValue) {
  std::vector<RecordedCall> calls;

  readLaunchArguments(Arguments{VALUE_OPTION, FLAG_OPTION},
                      recordingOptions(calls));

  EXPECT_EQ(calls,
            (std::vector<RecordedCall>{{.option = std::string(VALUE_OPTION),
                                        .value = std::string(FLAG_OPTION)}}));
}

/**
 * Given an argument that names no option
 * When the arguments are read
 * Then it is rejected
 */
TEST(LaunchArguments, RejectsAnUnknownArgument) {
  std::vector<RecordedCall> calls;

  EXPECT_THROW(
      readLaunchArguments(Arguments{UNKNOWN_ARGUMENT}, recordingOptions(calls)),
      UnknownLaunchArgumentException);
}

/**
 * Given a value without the option before it
 * When the arguments are read
 * Then it is rejected
 */
TEST(LaunchArguments, RejectsAValueWithoutItsOption) {
  std::vector<RecordedCall> calls;

  EXPECT_THROW(
      readLaunchArguments(Arguments{FIRST_VALUE}, recordingOptions(calls)),
      UnknownLaunchArgumentException);
}

/**
 * Given an option that takes a value as the last argument
 * When the arguments are read
 * Then the missing value is reported
 */
TEST(LaunchArguments, RejectsAValueOptionGivenLast) {
  std::vector<RecordedCall> calls;

  EXPECT_THROW(readLaunchArguments(Arguments{FLAG_OPTION, VALUE_OPTION},
                                   recordingOptions(calls)),
               MissingLaunchOptionValueException);
}

/**
 * Given an option whose record refuses its value
 * When the arguments are read
 * Then the record's exception reaches the caller
 */
TEST(LaunchArguments, LetsARecordRefuseItsValue) {
  const std::array options{
      LaunchOption{.name = VALUE_OPTION,
                   .takesValue = true,
                   .record = []([[maybe_unused]] std::string_view value) {
                     throw RefusedValue{};
                   }}};

  EXPECT_THROW(
      readLaunchArguments(Arguments{VALUE_OPTION, FIRST_VALUE}, options),
      RefusedValue);
}

/**
 * Given an option of the program and its value, an option a reader takes and
 * its value, then another option of the program
 * When the arguments are read
 * Then each option is recorded with its value, in the order given
 */
TEST(LaunchArguments, HandsTheReadersAnArgumentNoOptionNames) {
  std::vector<RecordedCall> calls;
  const std::array readers{recordingReader(calls)};

  readLaunchArguments(Arguments{VALUE_OPTION, FIRST_VALUE, READER_OPTION,
                                SECOND_VALUE, FLAG_OPTION},
                      recordingOptions(calls), readers);

  EXPECT_EQ(calls, (std::vector<RecordedCall>{
                       {.option = std::string(VALUE_OPTION),
                        .value = std::string(FIRST_VALUE)},
                       {.option = std::string(READER_OPTION),
                        .value = std::string(SECOND_VALUE)},
                       {.option = std::string(FLAG_OPTION), .value = ""}}));
}

/**
 * Given an option of the program that takes a value, then an option a reader
 * takes
 * When the arguments are read
 * Then the reader's option is the value, and the reader never gets it
 */
TEST(LaunchArguments, NeverHandsTheValueOfAnOptionToAReader) {
  std::vector<RecordedCall> calls;
  const std::array readers{recordingReader(calls)};

  readLaunchArguments(Arguments{VALUE_OPTION, READER_OPTION},
                      recordingOptions(calls), readers);

  EXPECT_EQ(calls,
            (std::vector<RecordedCall>{{.option = std::string(VALUE_OPTION),
                                        .value = std::string(READER_OPTION)}}));
}

/**
 * Given an argument that names no option and that no reader takes
 * When the arguments are read
 * Then it is rejected
 */
TEST(LaunchArguments, RejectsAnArgumentNoReaderTakes) {
  std::vector<RecordedCall> calls;
  const std::array readers{recordingReader(calls)};

  EXPECT_THROW(readLaunchArguments(Arguments{UNKNOWN_ARGUMENT},
                                   recordingOptions(calls), readers),
               UnknownLaunchArgumentException);
}

/**
 * Given an option a reader takes, given last without its value
 * When the arguments are read
 * Then the reader's exception reaches the caller
 */
TEST(LaunchArguments, LetsAReaderRefuseAnOption) {
  std::vector<RecordedCall> calls;
  const std::array readers{recordingReader(calls)};

  EXPECT_THROW(readLaunchArguments(Arguments{READER_OPTION},
                                   recordingOptions(calls), readers),
               RefusedValue);
}

/**
 * Given the arguments of a process, its name first
 * When they are read from the process
 * Then every argument after its name is kept, in order
 */
TEST(LaunchArguments, ProcessArgumentsLeaveTheProgramNameOut) {
  EXPECT_EQ(processArguments(static_cast<int>(PROCESS_ARGUMENTS.size()),
                             PROCESS_ARGUMENTS.data()),
            Arguments(PROCESS_ARGUMENTS.begin() + 1, PROCESS_ARGUMENTS.end()));
}

/**
 * Given a process launched with its name only
 * When its arguments are read
 * Then there is none
 */
TEST(LaunchArguments, ProcessLaunchedWithItsNameOnlyHasNoArgument) {
  EXPECT_TRUE(processArguments(static_cast<int>(PROGRAM_NAME_ONLY.size()),
                               PROGRAM_NAME_ONLY.data())
                  .empty());
}

/**
 * Given a program name
 * When the first lines of its usage are built
 * Then they name the program, then announce its options
 */
TEST(LaunchArguments, UsageHeaderNamesTheProgram) {
  EXPECT_EQ(usageHeader(TEST_PROGRAM_NAME), USAGE_HEADER_OF_TEST_PROGRAM);
}

/**
 * Given an option and its effect
 * When its lines of a usage are built
 * Then the option starts a line and its effect is indented on the next
 */
TEST(LaunchArguments, UsageEntryPutsTheEffectUnderTheOption) {
  EXPECT_EQ(usageEntry(USAGE_OPTION, USAGE_DESCRIPTION),
            USAGE_ENTRY_OF_USAGE_OPTION);
}

/**
 * Given the message of an argument a program cannot read
 * When the error to print is built
 * Then it names the program and the message, then tells how to see the usage
 */
TEST(LaunchArguments, LaunchErrorTextTellsHowToSeeTheUsage) {
  EXPECT_EQ(launchErrorText(TEST_PROGRAM_NAME, ERROR_MESSAGE),
            LAUNCH_ERROR_TEXT_OF_ERROR_MESSAGE);
}
