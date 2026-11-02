#include "ServerLeaderboardFixture.hpp"
#include <optional>
#include <string>
#include <string_view>
#include "LeaderboardAnswer.hpp"
#include "LeaderboardRequest.hpp"
#include "ServerLeaderboardTestConstants.hpp"

namespace rtype::client::testing {

network::LeaderboardRequest ServerLeaderboardFixture::askedRequest() {
  return network::LeaderboardRequest{
      .playerCount = ASKED_PILOTS, .difficulty = std::string{ASKED_DIFFICULTY}};
}

network::LeaderboardAnswer ServerLeaderboardFixture::answerFor(
    std::string_view difficulty) {
  return network::LeaderboardAnswer{
      .asked =
          network::LeaderboardRequest{.playerCount = ASKED_PILOTS,
                                      .difficulty = std::string{difficulty}},
      .rows = {network::LeaderboardRow{.rank = 1,
                                       .score = 1000,
                                       .playedAt = 0,
                                       .nickname = std::string{TOP_NICKNAME}}},
      .ownRow = std::nullopt};
}

}  // namespace rtype::client::testing
