#include "MasterClientFixture.hpp"
#include <functional>
#include <string>
#include "CreatedServer.hpp"
#include "GlobalLeaderboard.hpp"
#include "GlobalLeaderboardQuery.hpp"
#include "GlobalLeaderboardRow.hpp"
#include "MasterAccess.hpp"
#include "MasterClient.hpp"
#include "MasterClientTestConstants.hpp"
#include "MasterMessageTextTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "NetworkException.hpp"
#include "ServerCreation.hpp"

rtype::network::MasterClient MasterClientFixture::clientWith(
    const std::string& key) const {
  return rtype::network::MasterClient{
      rtype::network::MasterAccess{.url = master_.url(), .key = key}};
}

const RunningMaster& MasterClientFixture::master() const { return master_; }

rtype::network::CreatedServer MasterClientFixture::createdParis() const {
  return clientWith(std::string{})
      .createServer(rtype::network::ServerCreation{
          .name = std::string{TEST_SERVER_NAME}});
}

rtype::network::GlobalLeaderboardQuery
MasterClientFixture::parisGameLeaderboardQuery() {
  using rtype::network::testing::PARIS_DIFFICULTY;
  using rtype::network::testing::PLAYING_GAME_PLAYER_COUNT;
  using rtype::network::testing::SECOND_PILOT;
  return rtype::network::GlobalLeaderboardQuery{
      .difficulty = std::string{PARIS_DIFFICULTY},
      .playerCount = PLAYING_GAME_PLAYER_COUNT,
      .player = std::string{SECOND_PILOT},
      .limit = LEADERBOARD_ROWS_ASKED};
}

rtype::network::GlobalLeaderboard MasterClientFixture::parisGameLeaderboardOf(
    const rtype::network::CreatedServer& paris) {
  using rtype::network::GlobalLeaderboardRow;
  using rtype::network::testing::FINISHED_GAME_DATE;
  using rtype::network::testing::FIRST_PILOT;
  using rtype::network::testing::FIRST_PILOT_SCORE;
  using rtype::network::testing::FIRST_RANK;
  using rtype::network::testing::SECOND_PILOT;
  using rtype::network::testing::SECOND_PILOT_SCORE;
  using rtype::network::testing::SECOND_RANK;
  const GlobalLeaderboardRow second{.rank = SECOND_RANK,
                                    .nickname = std::string{SECOND_PILOT},
                                    .score = SECOND_PILOT_SCORE,
                                    .playedAt = FINISHED_GAME_DATE,
                                    .serverId = paris.id,
                                    .serverName = paris.name};
  return rtype::network::GlobalLeaderboard{
      .scores = {GlobalLeaderboardRow{.rank = FIRST_RANK,
                                      .nickname = std::string{FIRST_PILOT},
                                      .score = FIRST_PILOT_SCORE,
                                      .playedAt = FINISHED_GAME_DATE,
                                      .serverId = paris.id,
                                      .serverName = paris.name},
                 second},
      .player = second};
}

rtype::network::MasterRefusedException MasterClientFixture::refusalOf(
    const std::function<void()>& call) {
  try {
    call();
  } catch (const rtype::network::MasterRefusedException& refusal) {
    return refusal;
  }
  return rtype::network::MasterRefusedException{NO_REFUSAL, std::string{}};
}
