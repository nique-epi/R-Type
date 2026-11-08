#include "LeaderboardService.hpp"
#include "GlobalLeaderboard.hpp"
#include "LeaderboardQuery.hpp"
#include "MasterDatabase.hpp"
#include "ServerGameResult.hpp"

namespace rtype::master {

LeaderboardService::LeaderboardService(MasterDatabase& database)
    : results_(database) {}

bool LeaderboardService::keepResult(const ServerGameResult& result) {
  if (!result.game.vanilla) {
    return false;
  }
  results_.add(result);
  return true;
}

network::GlobalLeaderboard LeaderboardService::leaderboardOf(
    const LeaderboardQuery& query) {
  return results_.leaderboardOf(query);
}

}  // namespace rtype::master
