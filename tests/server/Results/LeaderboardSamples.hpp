#pragma once

#include "LeaderboardAnswer.hpp"
#include "LeaderboardRequest.hpp"

namespace rtype::server::testing {

/** @returns A request a player may send. */
[[nodiscard]] network::LeaderboardRequest someLeaderboardRequest();

/** @returns The answer to someLeaderboardRequest() when nobody played. */
[[nodiscard]] network::LeaderboardAnswer emptyLeaderboardAnswer();

}  // namespace rtype::server::testing
