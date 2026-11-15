#pragma once

#include <array>
#include <cstddef>
#include <string_view>

namespace rtype::logging {

constexpr std::string_view LEVEL_ENVIRONMENT_VARIABLE = "RT_LOG_LEVEL";
constexpr std::string_view NO_COLOR_ENVIRONMENT_VARIABLE = "NO_COLOR";
constexpr std::string_view PROJECT_NO_COLOR_ENVIRONMENT_VARIABLE =
    "RT_LOG_NO_COLOR";

constexpr std::string_view LEVEL_OPTION = "--log-level";
constexpr std::string_view FILE_OPTION = "--log-file";
constexpr std::string_view NO_FILE_OPTION = "--no-log-file";
constexpr std::string_view STDERR_OPTION = "--log-stderr";
constexpr std::string_view NO_STDERR_OPTION = "--no-log-stderr";
constexpr std::string_view COLOR_OPTION = "--log-color";
constexpr std::string_view FORMAT_OPTION = "--log-format";
constexpr std::string_view LOG_OPTION_PREFIX = "--log-";
constexpr std::string_view NO_LOG_OPTION_PREFIX = "--no-log-";

constexpr std::string_view LOGGING_LOGGER_NAME = "Logging";

/**
 * @brief How many lines wait for the thread of the journal at most: the places
 * are reserved when it starts, and a line queued while they are all taken is
 * dropped and counted. spdlog uses the same default.
 */
constexpr std::size_t JOURNAL_QUEUE_CAPACITY = 8192;

/** @brief The threads that write the journal: one, so lines keep the order
 * they were queued in. */
constexpr std::size_t JOURNAL_THREAD_COUNT = 1;

/**
 * @brief The first character of a text queued for the thread of the journal:
 * a line of the journal, or a line written to standard error as it is, such as
 * a line a game process wrote.
 */
constexpr char JOURNAL_LINE_KIND = 'L';
constexpr char STANDARD_ERROR_LINE_KIND = 'R';

/** @brief What follows the count of the lines the full queue of the journal
 * dropped: "3 lines were lost: the journal could not keep up". */
constexpr std::string_view LOST_LINES_TEXT =
    " lines were lost: the journal could not keep up";

constexpr std::string_view SERVER_LOG_FILE_NAME = "r-type_server.log";
constexpr std::string_view CLIENT_LOG_FILE_NAME = "r-type_client.log";
constexpr std::string_view MASTER_LOG_FILE_NAME = "r-type_master.log";
constexpr std::string_view NEXT_TO_THE_EXECUTABLE = " next to the executable";

constexpr std::string_view TRACE_NAME = "trace";
constexpr std::string_view DEBUG_NAME = "debug";
constexpr std::string_view INFO_NAME = "info";
constexpr std::string_view WARN_NAME = "warn";
constexpr std::string_view WARNING_NAME = "warning";
constexpr std::string_view ERROR_NAME = "error";
constexpr std::string_view SILENT_NAME = "silent";
constexpr std::string_view OFF_NAME = "off";
constexpr std::string_view NONE_NAME = "none";

/** @brief The values of FORMAT_OPTION: the journal file as text, or as JSON
 * Lines. */
constexpr std::string_view TEXT_FORMAT_NAME = "text";
constexpr std::string_view JSON_FORMAT_NAME = "json";

/** @brief The names of the members every JSON line of the journal starts with,
 * in this order. */
constexpr std::string_view TIME_KEY = "time";
constexpr std::string_view LEVEL_KEY = "level";
constexpr std::string_view LOGGER_KEY = "logger";
constexpr std::string_view MESSAGE_KEY = "message";

/** @brief The member that names the event of a line, after the four above. */
constexpr std::string_view EVENT_KEY = "event";

/** @brief The names no field of an event may take, since every line, or every
 * event, already has a member of that name. */
constexpr std::array RESERVED_KEYS{TIME_KEY, LEVEL_KEY, LOGGER_KEY, MESSAGE_KEY,
                                   EVENT_KEY};

/** @brief What ends the size of the message in a line queued for the thread
 * of the journal: "L5:hello{...}". */
constexpr char MESSAGE_SIZE_END = ':';

/** @brief The digits of the size of a queued message at most, so reading it
 * never overflows: a size_t of 64 bits holds 19 of them. */
constexpr std::size_t MESSAGE_SIZE_MAXIMUM_DIGITS = 18;
constexpr std::size_t DECIMAL_BASE = 10;

constexpr std::string_view TRACE_LABEL = "TRACE";
constexpr std::string_view DEBUG_LABEL = "DEBUG";
constexpr std::string_view INFO_LABEL = "INFO ";
constexpr std::string_view WARN_LABEL = "WARN ";
constexpr std::string_view ERROR_LABEL = "ERROR";
constexpr std::string_view SILENT_LABEL = "SILENT";
constexpr std::string_view UNKNOWN_LABEL = "?????";

constexpr std::string_view ANSI_RESET = "\x1b[0m";
constexpr std::string_view ANSI_GRAY = "\x1b[90m";
constexpr std::string_view ANSI_CYAN = "\x1b[36m";
constexpr std::string_view ANSI_GREEN = "\x1b[32m";
constexpr std::string_view ANSI_YELLOW = "\x1b[33m";
constexpr std::string_view ANSI_RED = "\x1b[31m";

/**
 * @brief What a ThrottledLog writes after the text of a line, around the number
 * of events the line stands for: "dropped a datagram (3 since the previous
 * line)".
 */
constexpr std::string_view EVENT_COUNT_OPENING = " (";
constexpr std::string_view EVENT_COUNT_CLOSING = " since the previous line)";

constexpr const char* TIMESTAMP_FORMAT = "%Y-%m-%dT%H:%M:%S";
constexpr int MILLISECOND_DIGITS = 3;
constexpr int DURATION_DECIMALS = 3;
constexpr double MICROSECONDS_PER_MILLISECOND = 1000.0;

}  // namespace rtype::logging
