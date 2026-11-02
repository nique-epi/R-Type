#pragma once

#include <cstddef>
#include "IncomingDatagram.hpp"

namespace rtype::server::testing {

/** @returns A datagram holding a LeaderboardRequest from the player at
 * @p index, under its session header. */
[[nodiscard]] network::IncomingDatagram leaderboardRequestFrom(
    std::size_t index);

/** @returns One datagram from the player at @p index holding a Disconnection,
 * then a LeaderboardRequest. */
[[nodiscard]] network::IncomingDatagram disconnectionThenLeaderboardRequestFrom(
    std::size_t index);

}  // namespace rtype::server::testing
