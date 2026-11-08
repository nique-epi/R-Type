#pragma once

#include <httplib.h>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include <vector>
#include "FinishedGame.hpp"
#include "FinishedGamePlayer.hpp"
#include "LeaderboardRoutesTestConstants.hpp"

/** @returns A guest of nickname @p nickname who scored @p score. */
[[nodiscard]] rtype::network::FinishedGamePlayer guestScoring(
    std::string_view nickname, std::uint32_t score);

/** @returns A vanilla game of "normal" for two players, ended at GAME_DATE,
 * whose rows are @p players. */
[[nodiscard]] rtype::network::FinishedGame normalGameOfTwo(
    std::vector<rtype::network::FinishedGamePlayer> players);

/** @returns The JSON body of @p answer, parsed; null when the master did
 * not answer. */
[[nodiscard]] nlohmann::json bodyOf(const httplib::Result& answer);

/** @returns The member @p member of each score of @p leaderboard, in order. */
template <typename Value>
[[nodiscard]] std::vector<Value> membersIn(const nlohmann::json& leaderboard,
                                           std::string_view member) {
  std::vector<Value> values;
  for (const nlohmann::json& row :
       leaderboard.at(std::string{SCORES_MEMBER_NAME})) {
    values.push_back(row.at(std::string{member}).get<Value>());
  }
  return values;
}
