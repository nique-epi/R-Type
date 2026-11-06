#pragma once

#include <chrono>
#include <cstdint>

namespace rtype::client::testing {

/** @brief The ports of two game processes a supervisor redirects to, on the
 * address of the supervisor. */
constexpr std::uint16_t GAME_PORT = 50123;
constexpr std::uint16_t OTHER_GAME_PORT = 50124;

/** @brief The time an attempt waits before the redirect, and the time it still
 * has after: the two make CONNECTION_ATTEMPT_TIMEOUT, five seconds. */
constexpr std::chrono::seconds BEFORE_THE_REDIRECT{4};
constexpr std::chrono::seconds AFTER_THE_REDIRECT{1};

/** @brief The game an attempt of the tests asks for by its number: game 3 of
 * the server. */
constexpr std::uint16_t NAMED_GAME_NUMBER = 3;

}  // namespace rtype::client::testing
