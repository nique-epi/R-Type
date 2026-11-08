#include "MasterLeaderboardFixture.hpp"
#include <cstdint>
#include <string>
#include "GlobalLeaderboard.hpp"
#include "GlobalLeaderboardRow.hpp"
#include "LeaderboardAnswer.hpp"
#include "LeaderboardRequest.hpp"
#include "MasterLeaderboard.hpp"
#include "MasterLeaderboardTestConstants.hpp"
#include "RecordingMasterLeaderboardReads.hpp"

namespace rtype::client::testing {

network::LeaderboardRequest MasterLeaderboardFixture::askedRequest() {
  return network::LeaderboardRequest{
      .playerCount = GLOBAL_ASKED_PILOTS,
      .difficulty = std::string{GLOBAL_ASKED_DIFFICULTY}};
}

network::GlobalLeaderboard MasterLeaderboardFixture::masterAnswer() {
  const network::GlobalLeaderboardRow nova{
      .rank = GLOBAL_SECOND_RANK,
      .nickname = std::string{GLOBAL_PLAYER},
      .score = GLOBAL_PLAYER_SCORE,
      .playedAt = GLOBAL_PLAYED_AT,
      .serverId = GLOBAL_SERVER_ID,
      .serverName = std::string{GLOBAL_SERVER_NAME}};
  return network::GlobalLeaderboard{
      .scores = {network::GlobalLeaderboardRow{
                     .rank = GLOBAL_FIRST_RANK,
                     .nickname = std::string{GLOBAL_TOP_PILOT},
                     .score = GLOBAL_TOP_SCORE,
                     .playedAt = GLOBAL_PLAYED_AT,
                     .serverId = GLOBAL_SERVER_ID,
                     .serverName = std::string{GLOBAL_SERVER_NAME}},
                 nova},
      .player = nova};
}

network::LeaderboardRow MasterLeaderboardFixture::kiraShown() {
  return network::LeaderboardRow{
      .rank = static_cast<std::uint32_t>(GLOBAL_FIRST_RANK),
      .score = GLOBAL_TOP_SCORE,
      .playedAt = GLOBAL_PLAYED_AT_SECONDS,
      .nickname = std::string{GLOBAL_TOP_PILOT}};
}

network::LeaderboardRow MasterLeaderboardFixture::novaShown() {
  return network::LeaderboardRow{
      .rank = static_cast<std::uint32_t>(GLOBAL_SECOND_RANK),
      .score = GLOBAL_PLAYER_SCORE,
      .playedAt = GLOBAL_PLAYED_AT_SECONDS,
      .nickname = std::string{GLOBAL_PLAYER}};
}

RecordingMasterLeaderboardReads& MasterLeaderboardFixture::reads() {
  return reads_;
}

MasterLeaderboard& MasterLeaderboardFixture::leaderboard() {
  return leaderboard_;
}

}  // namespace rtype::client::testing
