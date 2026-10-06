#pragma once

#include <string_view>

namespace rtype::logging {

constexpr std::string_view LEVEL_ENVIRONMENT_VARIABLE = "RT_LOG_LEVEL";
constexpr std::string_view NO_COLOR_ENVIRONMENT_VARIABLE = "NO_COLOR";
constexpr std::string_view PROJECT_NO_COLOR_ENVIRONMENT_VARIABLE =
    "RT_LOG_NO_COLOR";

constexpr std::string_view TRACE_NAME = "trace";
constexpr std::string_view DEBUG_NAME = "debug";
constexpr std::string_view INFO_NAME = "info";
constexpr std::string_view WARN_NAME = "warn";
constexpr std::string_view WARNING_NAME = "warning";
constexpr std::string_view ERROR_NAME = "error";
constexpr std::string_view SILENT_NAME = "silent";
constexpr std::string_view OFF_NAME = "off";
constexpr std::string_view NONE_NAME = "none";

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

constexpr const char* TIMESTAMP_FORMAT = "%Y-%m-%dT%H:%M:%S";
constexpr int MILLISECOND_DIGITS = 3;
constexpr int DURATION_DECIMALS = 3;
constexpr double MICROSECONDS_PER_MILLISECOND = 1000.0;

}  // namespace rtype::logging
