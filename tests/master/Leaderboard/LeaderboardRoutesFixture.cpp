#include "LeaderboardRoutesFixture.hpp"
#include <httplib.h>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include "CreatedServer.hpp"
#include "FinishedGame.hpp"
#include "LeaderboardRoutesFixtures.hpp"
#include "LeaderboardRoutesTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterMessageTextTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "RouteCall.hpp"
#include "RouteCallFixtures.hpp"
#include "ServersRoutesTestConstants.hpp"

using rtype::network::testing::FIRST_PILOT;
using rtype::network::testing::FIRST_PILOT_SCORE;
using rtype::network::testing::SECOND_PILOT;
using rtype::network::testing::SECOND_PILOT_SCORE;
using rtype::network::testing::THIRD_PILOT;

LeaderboardRoutesFixture::LeaderboardRoutesFixture()
    : paris_(master_.createServer(TEST_SERVER_NAME)),
      lyon_(master_.createServer(LYON_SERVER_NAME)) {}

void LeaderboardRoutesFixture::sendNormalHardAndAloneGames() const {
  static_cast<void>(sendResult(
      paris(), FIRST_GAME_ID,
      normalGameOfTwo({guestScoring(FIRST_PILOT, FIRST_PILOT_SCORE)})));
  rtype::network::FinishedGame hard =
      normalGameOfTwo({guestScoring(THIRD_PILOT, SECOND_PILOT_SCORE)});
  hard.difficulty = std::string{HARD_DIFFICULTY};
  static_cast<void>(sendResult(paris(), SECOND_GAME_ID, hard));
  rtype::network::FinishedGame alone =
      normalGameOfTwo({guestScoring(SECOND_PILOT, MIDDLE_SCORE)});
  alone.playerCount = ONE_PLAYER;
  static_cast<void>(sendResult(paris(), THIRD_GAME_ID, alone));
}

const rtype::network::CreatedServer& LeaderboardRoutesFixture::paris() const {
  return paris_;
}

const rtype::network::CreatedServer& LeaderboardRoutesFixture::lyon() const {
  return lyon_;
}

httplib::Result LeaderboardRoutesFixture::send(const RouteCall& call) const {
  return sendRouteCall(master_.port(), call);
}

httplib::Result LeaderboardRoutesFixture::sendResult(
    const rtype::network::CreatedServer& server, std::string_view gameId,
    const rtype::network::FinishedGame& game) const {
  const std::string path =
      std::string{OWN_RESULT_URL_PREFIX} + std::string{gameId};
  return send(RouteCall{.method = PUT_METHOD,
                        .path = path,
                        .key = server.key,
                        .body = rtype::network::finishedGameText(game)});
}

httplib::Result LeaderboardRoutesFixture::readLeaderboard(
    std::string_view query) const {
  const std::string path = std::string{LEADERBOARD_URL} + std::string{query};
  return send(RouteCall{
      .method = GET_METHOD_TEXT, .path = path, .key = {}, .body = {}});
}

nlohmann::json LeaderboardRoutesFixture::leaderboardOf(
    std::string_view query) const {
  return bodyOf(readLeaderboard(query));
}
