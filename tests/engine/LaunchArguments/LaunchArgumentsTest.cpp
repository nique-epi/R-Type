#include <gtest/gtest.h>
#include <array>
#include <exception>
#include <string>
#include <string_view>
#include <vector>
#include "EngineException.hpp"
#include "LaunchArguments.hpp"
#include "LaunchArgumentsTestConstants.hpp"
#include "LaunchOption.hpp"

using rtype::engine::launchErrorText;
using rtype::engine::LaunchOption;
using rtype::engine::MissingLaunchOptionValueException;
using rtype::engine::readLaunchArguments;
using rtype::engine::UnknownLaunchArgumentException;
using rtype::engine::usageEntry;
using rtype::engine::usageHeader;

namespace {

using Arguments = std::vector<std::string_view>;

/**
 * @brief One call of a record: the option it belongs to and the value it got.
 */
struct RecordedCall {
  std::string option;
  std::string value;

  bool operator==(const RecordedCall&) const = default;
};

/**
 * @brief What a record of the tests throws to refuse its value.
 */
class RefusedValue final : public std::exception {};

/**
 * @returns VALUE_OPTION and FLAG_OPTION, whose records append each call to
 *          @p calls.
 */
std::array<LaunchOption, 2> recordingOptions(std::vector<RecordedCall>& calls) {
  return {
      LaunchOption{.name = VALUE_OPTION,
                   .takesValue = true,
                   .record =
                       [&calls](std::string_view value) {
                         calls.push_back(
                             RecordedCall{.option = std::string(VALUE_OPTION),
                                          .value = std::string(value)});
                       }},
      LaunchOption{.name = FLAG_OPTION,
                   .takesValue = false,
                   .record =
                       [&calls](std::string_view value) {
                         calls.push_back(
                             RecordedCall{.option = std::string(FLAG_OPTION),
                                          .value = std::string(value)});
                       }},
  };
}

}  // namespace

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
