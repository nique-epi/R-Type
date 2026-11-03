#pragma once

#include <chrono>
#include "AllowanceVerdict.hpp"
#include "RateLimit.hpp"
#include "SenderAllowance.hpp"

namespace rtype::network {

/**
 * @brief Spends one datagram of @p allowance under @p limit at @p now, and
 * says whether it goes through.
 *
 * During a ban, the datagram is Banned and nothing changes: a sender that
 * keeps flooding does not lengthen its ban. Otherwise it is Allowed when it
 * comes no earlier than burst - 1 intervals before the theoretical arrival
 * time, which then moves one interval past the later of that time and
 * @p now. Otherwise it is dropped and counted in the window of drops, which
 * its first drop opens for BAN_WINDOW; the drop that reaches dropsBeforeBan
 * bans the sender until @p now + banDuration and forgets what it spent, so a
 * whole burst waits at the end of the ban. @p now never goes back from one
 * call to the next.
 */
[[nodiscard]] AllowanceVerdict spendAllowance(SenderAllowance& allowance,
                                              const RateLimit& limit,
                                              std::chrono::nanoseconds now);

/**
 * @returns Whether @p allowance is banned at @p now: before the end of its
 * last ban.
 */
[[nodiscard]] bool isBannedAt(const SenderAllowance& allowance,
                              std::chrono::nanoseconds now);

}  // namespace rtype::network
