#pragma once

#include <string_view>

/**
 * @brief What a journal line written at the error level holds between its
 * timestamp and its logger name: the operator reads it as an error.
 */
constexpr std::string_view ERROR_LINE_MARK = "[ERROR]";
