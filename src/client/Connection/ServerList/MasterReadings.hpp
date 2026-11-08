#pragma once

#include "GameListReading.hpp"
#include "GlobalLeaderboardReading.hpp"
#include "LatestValue.hpp"
#include "MasterAsks.hpp"
#include "ServerListReading.hpp"

namespace rtype::client {

/** @brief What the thread of a MasterLink publishes, newest first, and the
 * asks it answered so far, which only that thread reads and writes. */
struct MasterReadings {
  MasterAsks served;
  concurrency::LatestValue<ServerListReading> servers;
  concurrency::LatestValue<GameListReading> games;
  concurrency::LatestValue<GlobalLeaderboardReading> leaderboard;
};

}  // namespace rtype::client
