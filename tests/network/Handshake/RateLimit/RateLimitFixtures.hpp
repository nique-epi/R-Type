#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "AllowanceVerdict.hpp"
#include "Endpoint.hpp"
#include "RateLimit.hpp"
#include "RateLimitTestConstants.hpp"
#include "RequestAllowances.hpp"
#include "SenderAllowance.hpp"

namespace rtype::network::testing {

/**
 * @returns The verdicts of @p count datagrams of @p allowance under
 * TEST_RATE_LIMIT, all at @p now.
 */
[[nodiscard]] std::vector<AllowanceVerdict> spendTimes(
    SenderAllowance& allowance, std::size_t count,
    std::chrono::nanoseconds now);

/**
 * @returns The verdicts of @p count datagrams under TEST_RATE_LIMIT, one every
 * TEST_INTERVAL from START.
 */
[[nodiscard]] std::vector<AllowanceVerdict> spendEveryInterval(
    SenderAllowance& allowance, std::size_t count);

/** @returns An allowance that spent its whole burst at START. */
[[nodiscard]] SenderAllowance burstSpentAtStart();

/**
 * @returns An allowance banned at START: its burst, then TEST_DROPS_BEFORE_BAN
 * drops.
 */
[[nodiscard]] SenderAllowance bannedAtStart();

/**
 * @returns An allowance banned at START under @p limit: its burst, then the
 * drops that ban it.
 */
[[nodiscard]] SenderAllowance bannedAtStartUnder(const RateLimit& limit);

/**
 * @returns The verdicts of a whole burst of @p limit spent from @p allowance,
 * all at @p now.
 */
[[nodiscard]] std::vector<AllowanceVerdict> spendBurstUnder(
    SenderAllowance& allowance, const RateLimit& limit,
    std::chrono::nanoseconds now);

/** @returns The sender of the tests of the table, on @p port. */
[[nodiscard]] Endpoint limitedSender(std::uint16_t port = LIMITED_SENDER_PORT);

/** @returns Forged sender @p number: FORGED_ADDRESS, a port each. */
[[nodiscard]] Endpoint forgedSender(std::size_t number);

/**
 * @returns The verdicts of @p count datagrams of @p sender in @p allowances,
 * all at START.
 */
[[nodiscard]] std::vector<AllowanceVerdict> spendFromSender(
    RequestAllowances& allowances, const Endpoint& sender, std::size_t count);

/**
 * @brief Spends one datagram of each of the first @p count forged senders in
 * @p allowances, at @p now.
 */
void spendOnceFromEachForged(RequestAllowances& allowances, std::size_t count,
                             std::chrono::nanoseconds now);

}  // namespace rtype::network::testing
