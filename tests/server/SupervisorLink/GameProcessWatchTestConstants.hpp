#pragma once

#include <chrono>
#include <cstddef>
#include "TimeConstants.hpp"

/** @brief The delays of the requirement, in seconds, and the instants just
 * before each of them. */
constexpr rtype::engine::Duration FIVE_SECONDS = std::chrono::seconds(5);
constexpr rtype::engine::Duration JUST_UNDER_FIVE_SECONDS =
    std::chrono::milliseconds(4999);
constexpr rtype::engine::Duration TEN_SECONDS = std::chrono::seconds(10);
constexpr rtype::engine::Duration JUST_UNDER_TEN_SECONDS =
    std::chrono::milliseconds(9999);
constexpr rtype::engine::Duration THIRTY_SECONDS = std::chrono::seconds(30);
constexpr rtype::engine::Duration JUST_UNDER_THIRTY_SECONDS =
    std::chrono::milliseconds(29999);
constexpr rtype::engine::Duration ONE_MILLISECOND =
    std::chrono::milliseconds(1);

/** @brief The numbers of players the tests give the watch. */
constexpr std::size_t NO_PLAYER = 0;
constexpr std::size_t ONE_PLAYER = 1;
