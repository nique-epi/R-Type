#pragma once

#include <string_view>

/**
 * @brief An option of the tests that takes a value.
 */
constexpr std::string_view VALUE_OPTION = "-v";

/**
 * @brief An option of the tests that takes no value.
 */
constexpr std::string_view FLAG_OPTION = "-f";

/**
 * @brief A value given after VALUE_OPTION.
 */
constexpr std::string_view FIRST_VALUE = "first";

/**
 * @brief A second value given after VALUE_OPTION.
 */
constexpr std::string_view SECOND_VALUE = "second";

/**
 * @brief An argument that names no option of the tests.
 */
constexpr std::string_view UNKNOWN_ARGUMENT = "-x";

/**
 * @brief The program name the usage and error tests use.
 */
constexpr std::string_view TEST_PROGRAM_NAME = "r-type_test";

/**
 * @brief What usageHeader() returns for TEST_PROGRAM_NAME.
 */
constexpr std::string_view USAGE_HEADER_OF_TEST_PROGRAM =
    "Usage: r-type_test [options]\n\nOptions:\n";

/**
 * @brief An option with its value, as a usage shows it.
 */
constexpr std::string_view USAGE_OPTION = "-p <port>";

/**
 * @brief The effect of USAGE_OPTION, as a usage shows it.
 */
constexpr std::string_view USAGE_DESCRIPTION = "UDP port.";

/**
 * @brief What usageEntry() returns for USAGE_OPTION and USAGE_DESCRIPTION.
 */
constexpr std::string_view USAGE_ENTRY_OF_USAGE_OPTION =
    "  -p <port>\n      UDP port.\n";

/**
 * @brief An error message handed to launchErrorText().
 */
constexpr std::string_view ERROR_MESSAGE = "Unknown argument \"-x\"";

/**
 * @brief What launchErrorText() returns for TEST_PROGRAM_NAME and
 * ERROR_MESSAGE.
 */
constexpr std::string_view LAUNCH_ERROR_TEXT_OF_ERROR_MESSAGE =
    "r-type_test: Unknown argument \"-x\"\n"
    "Run r-type_test --help to see the options.\n";
