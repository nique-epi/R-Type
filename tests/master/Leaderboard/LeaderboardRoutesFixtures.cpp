#include "LeaderboardRoutesFixtures.hpp"
#include <httplib.h>
#include <cstdint>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "FinishedGame.hpp"
#include "FinishedGamePlayer.hpp"
#include "LeaderboardRoutesTestConstants.hpp"
#include "MasterMessageTextTestConstants.hpp"

nlohmann::json bodyOf(const httplib::Result& answer) {
  if (!answer) {
    return nlohmann::json{};
  }
  return nlohmann::json::parse(answer->body);
}

rtype::network::FinishedGamePlayer guestScoring(std::string_view nickname,
                                                std::uint32_t score) {
  return rtype::network::FinishedGamePlayer{.nickname = std::string{nickname},
                                            .accountId = std::nullopt,
                                            .score = score};
}

rtype::network::FinishedGame normalGameOfTwo(
    std::vector<rtype::network::FinishedGamePlayer> players) {
  return rtype::network::FinishedGame{
      .playedAt = GAME_DATE,
      .difficulty = std::string{rtype::network::testing::PARIS_DIFFICULTY},
      .playerCount = TWO_PLAYERS,
      .vanilla = true,
      .players = std::move(players)};
}
