#pragma once

#include <chrono>
#include <string_view>

/** @brief The names of the systems the tests add, and how long one run of each
 * takes. */
constexpr std::string_view FIRST_SYSTEM_NAME = "first";
constexpr std::string_view SECOND_SYSTEM_NAME = "second";
constexpr std::chrono::milliseconds FIRST_SYSTEM_TIME{3};
constexpr std::chrono::milliseconds SECOND_SYSTEM_TIME{5};

/** @brief How long two runs of the first system take. */
constexpr std::chrono::milliseconds FIRST_SYSTEM_TIME_OF_TWO_RUNS{6};
