#pragma once

#include <cstddef>
#include <string_view>

/**
 * @brief The name of the file the reports are about.
 */
constexpr std::string_view LOGGED_FILE_NAME = "server.cfg";

/**
 * @brief The name of the logger the tests log with.
 */
constexpr std::string_view LOGGED_MODULE_NAME = "Test";

/**
 * @brief What the journal says when the file was not found.
 */
constexpr std::string_view NOT_FOUND_TEXT =
    "server.cfg not found, the defaults apply";

/**
 * @brief How many commands the report of a found file counts.
 */
constexpr std::size_t LOGGED_EXECUTED_LINES = 2;

/**
 * @brief What the journal says about that count.
 */
constexpr std::string_view EXECUTED_TEXT = "2 commands run";

/**
 * @brief A refusal the report holds.
 */
constexpr std::string_view LOGGED_REFUSAL =
    "server.cfg:3: command 'set' refused";

/**
 * @brief What marks a warning in a line of the journal.
 */
constexpr std::string_view WARNING_LEVEL_TEXT = "WARN";
