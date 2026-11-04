#pragma once

#include <chrono>
#include <cstddef>
#include <vector>
#include "AllowanceVerdict.hpp"
#include "RateLimit.hpp"

namespace rtype::network::testing {

/**
 * @returns When a joining client sends for @p duration: a request every
 * CONNECTION_REQUEST_INTERVAL and its answer CHALLENGE_ROUND_TRIP later.
 */
[[nodiscard]] std::vector<std::chrono::nanoseconds> joiningClientTimes(
    std::chrono::nanoseconds duration);

/**
 * @returns When a client whose server stalls sends: a request every
 * CONNECTION_REQUEST_INTERVAL until CONNECTION_ATTEMPT_TIMEOUT, then, all
 * CHALLENGE_ROUND_TRIP after the last one, the answer to the challenge of each
 * of them.
 */
[[nodiscard]] std::vector<std::chrono::nanoseconds> stalledClientTimes();

/**
 * @returns When a player sends for @p duration: PLAYER_INPUTS_PER_SECOND
 * inputs evenly spread over each second, and a heartbeat at its start.
 */
[[nodiscard]] std::vector<std::chrono::nanoseconds> playerTimes(
    std::chrono::nanoseconds duration);

/**
 * @returns The verdicts of one sender sending at each of @p times under
 * @p limit.
 */
[[nodiscard]] std::vector<AllowanceVerdict> verdictsAt(
    const RateLimit& limit, const std::vector<std::chrono::nanoseconds>& times);

/** @returns How many of @p verdicts are Allowed. */
[[nodiscard]] std::size_t allowedCount(
    const std::vector<AllowanceVerdict>& verdicts);

}  // namespace rtype::network::testing
