#include "LeaderboardController.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "HttpConstants.hpp"
#include "KnownServer.hpp"
#include "LeaderboardControllerConstants.hpp"
#include "LeaderboardQuery.hpp"
#include "MasterDatabase.hpp"
#include "MasterMessageConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterRouteConstants.hpp"
#include "PageRequest.hpp"
#include "ProtocolConstants.hpp"
#include "QueryBounds.hpp"
#include "RouteAnswers.hpp"
#include "ServerGameResult.hpp"
#include "ServersControllerConstants.hpp"
#include "ServersService.hpp"
#include "WireTypes.hpp"

namespace rtype::master {

namespace {

/** @returns The id of the game the path of @p request names, nothing when it
 * is no id of a game. */
std::optional<std::string> gameIdOf(const network::ApiRequest& request) {
  const auto parameter =
      request.pathParameters.find(std::string{network::GAME_ID_PATH_PARAMETER});
  if (parameter == request.pathParameters.end() ||
      !network::isWellFormedGameId(parameter->second)) {
    return std::nullopt;
  }
  return parameter->second;
}

network::ApiResponse invalidGameIdRefusal() {
  return refusal(BAD_REQUEST_STATUS, std::string{GAME_ID_DETAIL_START} +
                                         std::to_string(network::GAME_ID_SIZE) +
                                         std::string{GAME_ID_DETAIL_END});
}

/** @returns The answer of a query of the leaderboard out of its bounds: 400,
 * whose detail gives every bound. */
network::ApiResponse invalidQueryRefusal() {
  const std::string nicknameSizes =
      std::to_string(network::MINIMUM_NICKNAME_SIZE) +
      std::string{DETAIL_RANGE_JOIN} +
      std::to_string(network::MAXIMUM_NICKNAME_SIZE);
  return refusal(BAD_REQUEST_STATUS,
                 std::string{DIFFICULTY_DETAIL_START} + nicknameSizes +
                     std::string{PLAYER_COUNT_DETAIL_START} +
                     std::to_string(PLAYER_COUNT_BOUNDS.lowest) +
                     std::string{DETAIL_RANGE_JOIN} +
                     std::to_string(PLAYER_COUNT_BOUNDS.highest) +
                     std::string{SERVER_DETAIL_START} +
                     std::to_string(SERVER_BOUNDS.lowest) +
                     std::string{PLAYER_DETAIL_START} + nicknameSizes +
                     std::string{BYTES_DETAIL_END});
}

/** @returns The number @p text writes within @p bounds; nothing when there is
 * no text, or a text out of the bounds. */
std::optional<std::int64_t> numberIn(const std::optional<std::string>& text,
                                     QueryBounds bounds) {
  if (!text.has_value()) {
    return std::nullopt;
  }
  return boundedNumberIn(*text, bounds);
}

/** @returns The leaderboard the query of @p request asks for, read a @p page
 * at a time; nothing when the difficulty or the number of players is missing,
 * or a parameter is out of its bounds. */
std::optional<LeaderboardQuery> queryOf(const network::ApiRequest& request,
                                        PageRequest page) {
  const std::optional<std::string> difficulty =
      queryTextOf(request, network::DIFFICULTY_QUERY_PARAMETER);
  const std::optional<std::int64_t> playerCount =
      numberIn(queryTextOf(request, network::PLAYER_COUNT_QUERY_PARAMETER),
               PLAYER_COUNT_BOUNDS);
  const std::optional<std::string> server =
      queryTextOf(request, network::SERVER_QUERY_PARAMETER);
  const std::optional<std::int64_t> serverId = numberIn(server, SERVER_BOUNDS);
  const std::optional<std::string> player =
      queryTextOf(request, network::PLAYER_QUERY_PARAMETER);
  if (!difficulty.has_value() || !network::isValidNickname(*difficulty) ||
      !playerCount.has_value() || server.has_value() != serverId.has_value() ||
      (player.has_value() && !network::isValidNickname(*player))) {
    return std::nullopt;
  }
  return LeaderboardQuery{
      .difficulty = *difficulty,
      .playerCount = static_cast<std::uint8_t>(*playerCount),
      .serverId = serverId,
      .player = player,
      .page = page};
}

}  // namespace

LeaderboardController::LeaderboardController(MasterDatabase& database,
                                             ServersService& servers)
    : service_(database), servers_(servers) {}

network::ApiResponse LeaderboardController::keepOwnResult(
    const network::ApiRequest& request) {
  return answerAsServerOfKey(
      servers_, request, [this, &request](const KnownServer& server) {
        const std::optional<std::string> gameId = gameIdOf(request);
        if (!gameId.has_value()) {
          return invalidGameIdRefusal();
        }
        if (!service_.keepResult(ServerGameResult{
                .serverId = server.id,
                .gameId = *gameId,
                .game = network::finishedGameFromText(request.body)})) {
          return refusal(BAD_REQUEST_STATUS, NOT_VANILLA_DETAIL);
        }
        return network::ApiResponse{.status = NO_CONTENT_STATUS, .body = {}};
      });
}

network::ApiResponse LeaderboardController::readLeaderboard(
    const network::ApiRequest& request) {
  const std::optional<PageRequest> page = pageOf(request);
  if (!page.has_value()) {
    return invalidPageRefusal();
  }
  const std::optional<LeaderboardQuery> query = queryOf(request, *page);
  if (!query.has_value()) {
    return invalidQueryRefusal();
  }
  return network::ApiResponse{
      .status = OK_STATUS,
      .body = network::globalLeaderboardText(service_.leaderboardOf(*query))};
}

}  // namespace rtype::master
