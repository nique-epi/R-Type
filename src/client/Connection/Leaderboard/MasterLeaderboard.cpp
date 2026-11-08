#include "MasterLeaderboard.hpp"
#include <algorithm>
#include <cstdint>
#include <limits>
#include <optional>
#include <string>
#include "GlobalLeaderboard.hpp"
#include "GlobalLeaderboardQuery.hpp"
#include "GlobalLeaderboardReading.hpp"
#include "GlobalLeaderboardRow.hpp"
#include "IMasterLeaderboardReads.hpp"
#include "LeaderboardAnswer.hpp"
#include "LeaderboardPage.hpp"
#include "LeaderboardPhase.hpp"
#include "LeaderboardRequest.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::client {

namespace {

/** @returns @p row as the leaderboard screen shows a row: a rank past the
 * largest a row holds shows as that largest. */
network::LeaderboardRow rowOf(const network::GlobalLeaderboardRow& row) {
  return network::LeaderboardRow{
      .rank = static_cast<std::uint32_t>(std::min<std::int64_t>(
          row.rank, std::numeric_limits<std::uint32_t>::max())),
      .score = row.score,
      .playedAt = row.playedAt.time_since_epoch().count(),
      .nickname = row.nickname};
}

}  // namespace

MasterLeaderboard::MasterLeaderboard(IMasterLeaderboardReads& master)
    : master_(&master) {}

void MasterLeaderboard::ask(const network::LeaderboardRequest& request) {
  asked_ = master_->askLeaderboard(network::GlobalLeaderboardQuery{
      .difficulty = request.difficulty,
      .playerCount = request.playerCount,
      .player = player_,
      .limit = static_cast<std::int64_t>(network::MAXIMUM_LEADERBOARD_ROWS)});
  waiting_ = true;
  become(LeaderboardPhase::Asking);
}

void MasterLeaderboard::stopAsking() noexcept { waiting_ = false; }

const LeaderboardPage& MasterLeaderboard::page() const { return page_; }

void MasterLeaderboard::namePlayer(const std::string& nickname) {
  player_ = network::isValidNickname(nickname)
                ? std::optional<std::string>{nickname}
                : std::nullopt;
}

void MasterLeaderboard::update() {
  const std::optional<GlobalLeaderboardReading> reading =
      master_->leaderboard();
  if (!waiting_ || !reading.has_value() || reading->ask != asked_) {
    return;
  }
  waiting_ = false;
  if (reading->leaderboard.has_value()) {
    show(*reading->leaderboard);
  } else {
    become(LeaderboardPhase::OutOfReach);
  }
}

void MasterLeaderboard::show(const network::GlobalLeaderboard& leaderboard) {
  become(LeaderboardPhase::Answered);
  for (const network::GlobalLeaderboardRow& row : leaderboard.scores) {
    page_.rows.push_back(rowOf(row));
  }
  if (leaderboard.player.has_value()) {
    page_.ownRow = rowOf(*leaderboard.player);
  }
}

void MasterLeaderboard::become(LeaderboardPhase phase) {
  page_.rows.clear();
  page_.ownRow.reset();
  page_.phase = phase;
  ++page_.revision;
}

}  // namespace rtype::client
