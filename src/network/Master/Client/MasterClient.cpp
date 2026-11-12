#include "MasterClient.hpp"
#include <httplib.h>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include "CreatedServer.hpp"
#include "FinishedGame.hpp"
#include "GameListing.hpp"
#include "GlobalLeaderboard.hpp"
#include "GlobalLeaderboardQuery.hpp"
#include "MasterAccess.hpp"
#include "MasterClientConstants.hpp"
#include "MasterHttpConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterRoute.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"
#include "NetworkException.hpp"
#include "ServerCreation.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerListing.hpp"
#include "ServerRegistration.hpp"

// NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)
namespace rtype::network {

namespace {

/** @returns The request of @p route carrying @p body in JSON. */
httplib::Request requestOf(MasterRoute route, const std::string& body) {
  httplib::Request request;
  request.method = std::string{methodNameOf(route.method)};
  request.path = apiPathOf(route);
  request.body = body;
  request.set_header(std::string{CONTENT_TYPE_HEADER},
                     std::string{JSON_CONTENT_TYPE});
  return request;
}

/** @returns The request of @p route, which has no body, for @p path. */
httplib::Request readRequestOf(MasterRoute route, const std::string& path) {
  httplib::Request request;
  request.method = std::string{methodNameOf(route.method)};
  request.path = path;
  return request;
}

/** @returns The path of the page of LIST_SERVERS_ROUTE that starts after
 * @p offset servers: `/api/servers?limit=100&offset=200`. */
std::string serverPagePathOf(std::int64_t offset) {
  return apiPathOf(LIST_SERVERS_ROUTE) + std::string{QUERY_START} +
         std::string{LIMIT_QUERY_PARAMETER} +
         std::string{QUERY_VALUE_SEPARATOR} +
         std::to_string(MAXIMUM_PAGE_SIZE) +
         std::string{QUERY_PARAMETER_SEPARATOR} +
         std::string{OFFSET_QUERY_PARAMETER} +
         std::string{QUERY_VALUE_SEPARATOR} + std::to_string(offset);
}

/** @returns The query of @p query, which cpp-httplib encodes after the path:
 * `limit`, then `difficulty`, `playerCount` and `player` when it names them.
 */
httplib::Params leaderboardParamsOf(const GlobalLeaderboardQuery& query) {
  httplib::Params params{
      {std::string{LIMIT_QUERY_PARAMETER}, std::to_string(query.limit)}};
  if (query.difficulty.has_value()) {
    params.emplace(std::string{DIFFICULTY_QUERY_PARAMETER}, *query.difficulty);
  }
  if (query.playerCount.has_value()) {
    params.emplace(
        std::string{PLAYER_COUNT_QUERY_PARAMETER},
        std::to_string(static_cast<unsigned int>(*query.playerCount)));
  }
  if (query.player.has_value()) {
    params.emplace(std::string{PLAYER_QUERY_PARAMETER}, *query.player);
  }
  return params;
}

/** @brief Sends @p key as Bearer with @p request, unless it is empty. */
void authorize(httplib::Request& request, const std::string& key) {
  if (!key.empty()) {
    request.set_header(std::string{AUTHORIZATION_HEADER},
                       std::string{BEARER_SCHEME} +
                           std::string{BEARER_SCHEME_SEPARATOR} + key);
  }
}

/**
 * @returns A client of the master at @p url.
 * @throws InvalidMasterUrlException for a url cpp-httplib refuses: it throws
 * for a scheme it does not speak, and builds a client with nothing inside,
 * which is_valid() reports, for a port outside 1 to 65535.
 */
std::unique_ptr<httplib::Client> clientOf(const std::string& url) {
  std::unique_ptr<httplib::Client> client;
  try {
    client = std::make_unique<httplib::Client>(url);
  } catch (const std::invalid_argument&) {
    throw InvalidMasterUrlException(url);
  }
  if (!client->is_valid()) {
    throw InvalidMasterUrlException(url);
  }
  return client;
}

}  // namespace

MasterClient::MasterClient(MasterAccess access)
    : key_(std::move(access.key)), client_(clientOf(access.url)) {
  client_->set_keep_alive(true);
  client_->set_connection_timeout(MASTER_CONNECTION_TIMEOUT);
  client_->set_read_timeout(MASTER_TRANSFER_TIMEOUT);
  client_->set_write_timeout(MASTER_TRANSFER_TIMEOUT);
}

MasterClient::~MasterClient() = default;

CreatedServer MasterClient::createServer(const ServerCreation& creation) {
  return createdServerFromText(
      answerOf(CREATE_SERVER_ROUTE, serverCreationText(creation)));
}

void MasterClient::registerServer(const ServerRegistration& registration) {
  static_cast<void>(
      answerOf(REPLACE_OWN_SERVER_ROUTE, registrationText(registration)));
}

void MasterClient::sendHeartbeat(const ServerHeartbeat& heartbeat) {
  static_cast<void>(
      answerOf(REPLACE_OWN_HEARTBEAT_ROUTE, heartbeatText(heartbeat)));
}

void MasterClient::sendResult(const std::string& gameId,
                              const FinishedGame& game) {
  httplib::Request request =
      requestOf(KEEP_OWN_RESULT_ROUTE, finishedGameText(game));
  request.path = gamePathOf(KEEP_OWN_RESULT_ROUTE, gameId);
  static_cast<void>(send(std::move(request)));
}

GameListing MasterClient::listGames(std::int64_t serverId) {
  return gameListingFromText(
      send(readRequestOf(LIST_SERVER_GAMES_ROUTE,
                         serverPathOf(LIST_SERVER_GAMES_ROUTE, serverId))));
}

ServerListing MasterClient::listServers() {
  ServerListing every;
  for (std::int64_t page = 0; page < MAXIMUM_LISTED_SERVER_PAGES; ++page) {
    const ServerListing read = serverListingFromText(send(readRequestOf(
        LIST_SERVERS_ROUTE, serverPagePathOf(page * MAXIMUM_PAGE_SIZE))));
    every.servers.insert(every.servers.end(), read.servers.begin(),
                         read.servers.end());
    if (std::cmp_less(read.servers.size(), MAXIMUM_PAGE_SIZE)) {
      break;
    }
  }
  return every;
}

GlobalLeaderboard MasterClient::readLeaderboard(
    const GlobalLeaderboardQuery& query) {
  httplib::Request request =
      readRequestOf(READ_LEADERBOARD_ROUTE, apiPathOf(READ_LEADERBOARD_ROUTE));
  request.params = leaderboardParamsOf(query);
  return globalLeaderboardFromText(send(std::move(request)));
}

std::string MasterClient::answerOf(MasterRoute route, const std::string& body) {
  return send(requestOf(route, body));
}

std::string MasterClient::send(httplib::Request request) {
  authorize(request, key_);
  const httplib::Result result = client_->send(request);
  if (!result) {
    throw MasterUnreachableException(httplib::to_string(result.error()));
  }
  if (result->status >= LOWEST_ERROR_STATUS) {
    throw MasterRefusedException(result->status, problemDetailOf(result->body));
  }
  return result->body;
}

}  // namespace rtype::network
// NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks)
