#pragma once

#include <string_view>

/**
 * @brief The name of a command the tests register.
 */
constexpr std::string_view ECHO_COMMAND = "echo";

/**
 * @brief The name of a second command the tests register.
 */
constexpr std::string_view ALPHA_COMMAND = "alpha";

/**
 * @brief The name of a command no test registers.
 */
constexpr std::string_view UNREGISTERED_COMMAND = "missing";

/**
 * @brief The first word after a command name.
 */
constexpr std::string_view FIRST_ARGUMENT = "one";

/**
 * @brief The second word after a command name.
 */
constexpr std::string_view SECOND_ARGUMENT = "two";

/**
 * @brief The reason a refusing command gives.
 */
constexpr std::string_view REFUSAL_REASON = "no thanks";

/**
 * @brief The shape of the arguments of the echo command, as help shows it.
 */
constexpr std::string_view ECHO_ARGUMENTS = "<word> <word>";

/**
 * @brief What the echo command does, as help shows it.
 */
constexpr std::string_view ECHO_HELP = "repeats its words";

/**
 * @brief What the alpha command does, as help shows it.
 */
constexpr std::string_view ALPHA_HELP = "comes first";

/**
 * @brief A line made of spaces.
 */
constexpr std::string_view BLANK_LINE = "   ";

/**
 * @brief A line that is only a comment.
 */
constexpr std::string_view COMMENT_LINE = "# echo one two";
