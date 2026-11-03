#pragma once

#include <vector>
#include "Games.hpp"
#include "GamesCensus.hpp"
#include "IClock.hpp"
#include "SystemTime.hpp"

namespace rtype::server {

/** @returns What @p games hold now. Use it from the simulation thread only. */
[[nodiscard]] GamesCensus censusOf(const Games& games);

/**
 * @brief Has the systems of every game of @p games read @p clock (see
 * SystemScheduler::measureWith()); nullptr stops them.
 * @param clock Must stay valid until the next call.
 */
void measureSystemsOf(const Games& games, const engine::IClock* clock);

/**
 * @returns How long each system ran since the previous call, added up by name
 * over every game of @p games, each name once, in the order first met.
 */
[[nodiscard]] std::vector<engine::SystemTime> takeSystemTimesOf(
    const Games& games);

}  // namespace rtype::server
