#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

/**
 * @brief The name of the configuration file the tests write.
 */
constexpr std::string_view CONFIGURATION_FILE = "test.cfg";

/**
 * @brief The name of a configuration file no test writes.
 */
constexpr std::string_view MISSING_CONFIGURATION_FILE = "absent.cfg";

/**
 * @brief A file of two lines that set the port and the nickname.
 */
constexpr std::string_view TWO_SETTINGS_FILE =
    "# the server\nset port 5000\nset nickname ace\n";

/**
 * @brief A file whose second line sets a port out of bounds.
 */
constexpr std::string_view FILE_WITH_A_BAD_LINE =
    "set nickname ace\nset port 99999\nset port 5000\n";

/**
 * @brief A file that sets the port, with Windows line ends.
 */
constexpr std::string_view FILE_WITH_WINDOWS_LINE_ENDS =
    "set port 5000\r\nset nickname ace\r\n";

/**
 * @brief A file that sets the port to 5000.
 */
constexpr std::string_view FILE_SETTING_PORT = "set port 5000\n";

/**
 * @brief The line a launch option forces on the port.
 */
constexpr std::string_view FORCED_PORT_LINE = "set port 6000";

/**
 * @brief A forced line that sets a port out of bounds.
 */
constexpr std::string_view FORCED_BAD_PORT_LINE = "set port 99999";

/**
 * @brief What names a forced line in a refusal.
 */
constexpr std::string_view FORCED_LINE_ORIGIN_TEXT = "launch option";

/**
 * @brief The port the file gives.
 */
constexpr std::int64_t FILE_PORT = 5000;

/**
 * @brief The port the launch option forces.
 */
constexpr std::int64_t FORCED_PORT = 6000;

/**
 * @brief The number of lines of TWO_SETTINGS_FILE that run.
 */
constexpr std::size_t TWO_SETTINGS_FILE_RUNNABLE_LINES = 2;

/**
 * @brief The number of the line FILE_WITH_A_BAD_LINE gets wrong.
 */
constexpr std::string_view BAD_LINE_POSITION = ":2:";
