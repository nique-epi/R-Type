#pragma once

#include <cstdint>
#include "MillisecondDate.hpp"
#include "RouletteDurations.hpp"
#include "ScheduledRound.hpp"

namespace rtype::master {

/**
 * @returns The round of the casino at @p now, read on the clock alone: rounds
 * follow one another from 1970, each the sum of @p durations long, so round n
 * starts at n times that sum, and every room plays the same round. Its bets
 * are open from its start to closesAt excluded, its wheel spins from closesAt
 * to resultAt excluded, its result shows from resultAt to endsAt excluded,
 * the start of round n + 1. Each duration is longer than none.
 */
[[nodiscard]] ScheduledRound scheduledRoundAt(
    network::MillisecondDate now, const RouletteDurations& durations);

/** @returns The first round of @p scheduled whose bets are still open: its
 * own while they are, the next one once they closed. */
[[nodiscard]] std::uint64_t firstOpenRound(const ScheduledRound& scheduled);

/** @returns The clock of the system, to the millisecond. */
[[nodiscard]] network::MillisecondDate currentMillisecond();

}  // namespace rtype::master
