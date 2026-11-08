#include "RecordingMasterLeaderboardReads.hpp"
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>
#include "GlobalLeaderboard.hpp"
#include "GlobalLeaderboardQuery.hpp"
#include "GlobalLeaderboardReading.hpp"

std::uint64_t RecordingMasterLeaderboardReads::askLeaderboard(
    const rtype::network::GlobalLeaderboardQuery& query) {
  queries_.push_back(query);
  return queries_.size();
}

std::optional<rtype::client::GlobalLeaderboardReading>
RecordingMasterLeaderboardReads::leaderboard() const {
  return reading_;
}

void RecordingMasterLeaderboardReads::answer(
    std::uint64_t ask,
    std::optional<rtype::network::GlobalLeaderboard> leaderboard) {
  reading_ = rtype::client::GlobalLeaderboardReading{
      .ask = ask, .leaderboard = std::move(leaderboard)};
}

const std::vector<rtype::network::GlobalLeaderboardQuery>&
RecordingMasterLeaderboardReads::queries() const {
  return queries_;
}
