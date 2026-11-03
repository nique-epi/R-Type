#include "LeaderboardSamples.hpp"
#include <optional>
#include "LeaderboardAnswer.hpp"
#include "LeaderboardRequest.hpp"

namespace rtype::server::testing {

network::LeaderboardRequest someLeaderboardRequest() {
  return network::LeaderboardRequest{.playerCount = 2, .difficulty = "normal"};
}

network::LeaderboardAnswer emptyLeaderboardAnswer() {
  return network::LeaderboardAnswer{
      .asked = someLeaderboardRequest(), .rows = {}, .ownRow = std::nullopt};
}

}  // namespace rtype::server::testing
