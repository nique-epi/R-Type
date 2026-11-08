#pragma once

#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "LeaderboardService.hpp"
#include "MasterDatabase.hpp"
#include "ServersService.hpp"

namespace rtype::master {

/**
 * @brief The routes of the results and of the global leaderboard: what reads
 * their request and writes their answer. A result is sent by the server of a
 * key, as the other routes of a server; the leaderboard is open to everyone.
 * Every refusal is a problem document: 401 without a valid key (with
 * `WWW-Authenticate: Bearer`), 400 naming what was refused.
 */
class LeaderboardController {
 public:
  /** @param servers Tells the server of a key; it must outlive the
   * controller. */
  LeaderboardController(MasterDatabase& database, ServersService& servers);

  /**
   * @returns The answer of `PUT /api/servers/me/results/{gameId}`: 204 once
   * the result is kept, or was kept already; 400 for an id that is no id of a
   * game, a body whose member is missing or out of its bounds, or a game that
   * was not vanilla. The key is checked first, then the id, then the body.
   */
  [[nodiscard]] network::ApiResponse keepOwnResult(
      const network::ApiRequest& request);

  /**
   * @returns The answer of `GET /api/leaderboard`: 200, the page asked for
   * and the row of the player the query names; 400 when the difficulty or the
   * number of players is missing, or a parameter is out of its bounds.
   */
  [[nodiscard]] network::ApiResponse readLeaderboard(
      const network::ApiRequest& request);

 private:
  LeaderboardService service_;
  ServersService& servers_;
};

}  // namespace rtype::master
