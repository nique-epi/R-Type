#include "ServersService.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include "CreatedServer.hpp"
#include "CreationAttempt.hpp"
#include "CreationQuota.hpp"
#include "GameListing.hpp"
#include "HeartbeatRecord.hpp"
#include "HostedGame.hpp"
#include "HostedGameState.hpp"
#include "KnownServer.hpp"
#include "LastHeartbeat.hpp"
#include "ListedGame.hpp"
#include "ListedGameState.hpp"
#include "MasterDatabase.hpp"
#include "MasterMessageConstants.hpp"
#include "MasterMessageText.hpp"
#include "NetworkException.hpp"
#include "NewServer.hpp"
#include "PageRequest.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerKey.hpp"
#include "ServerLimits.hpp"
#include "ServerListing.hpp"
#include "ServerRecord.hpp"
#include "ServersServiceConstants.hpp"
#include "StatusChangeListing.hpp"

namespace rtype::master {

namespace {

std::string pointerOfGame(std::size_t index, std::string_view member) {
  return (nlohmann::json::json_pointer{} / std::string{network::GAMES_MEMBER} /
          index / std::string{member})
      .to_string();
}

/**
 * @throws network::InvalidMasterMessageException naming the state of the game
 * @p game, the one at @p index, when it says the game waits for players but the
 * seats do not agree: Full without every seat taken, or Waiting with every seat
 * taken.
 */
void requireStateToFitPlayers(const network::HostedGame& game,
                              const ServerLimits& limits, std::size_t index) {
  const bool seatsTaken = game.players.size() == limits.maximumPlayersPerGame;
  const bool refused =
      (game.state == network::HostedGameState::Full && !seatsTaken) ||
      (game.state == network::HostedGameState::Waiting && seatsTaken);
  if (refused) {
    throw network::InvalidMasterMessageException(
        pointerOfGame(index, network::STATE_MEMBER));
  }
}

/**
 * @throws network::InvalidMasterMessageException naming the first member of
 * @p heartbeat beyond @p limits.
 */
void requireWithinLimits(const network::ServerHeartbeat& heartbeat,
                         const ServerLimits& limits) {
  if (heartbeat.games.size() > limits.maximumGames) {
    throw network::InvalidMasterMessageException(
        (nlohmann::json::json_pointer{} / std::string{network::GAMES_MEMBER})
            .to_string());
  }
  for (std::size_t index = 0; index < heartbeat.games.size(); ++index) {
    const network::HostedGame& game = heartbeat.games.at(index);
    if (game.number > limits.maximumGames) {
      throw network::InvalidMasterMessageException(
          pointerOfGame(index, network::NUMBER_MEMBER));
    }
    if (game.players.size() > limits.maximumPlayersPerGame) {
      throw network::InvalidMasterMessageException(
          pointerOfGame(index, network::PLAYERS_MEMBER));
    }
    requireStateToFitPlayers(game, limits, index);
  }
}

network::ListedGameState listedStateOf(network::HostedGameState state) {
  switch (state) {
    case network::HostedGameState::Waiting:
      return network::ListedGameState::Waiting;
    case network::HostedGameState::Full:
      return network::ListedGameState::Full;
    case network::HostedGameState::Starting:
    case network::HostedGameState::Playing:
    case network::HostedGameState::Finished:
      return network::ListedGameState::InGame;
  }
  return network::ListedGameState::InGame;
}

network::ListedGame listedGameOf(const network::HostedGame& game) {
  return network::ListedGame{
      .number = game.number,
      .name = game.name,
      .state = listedStateOf(game.state),
      .playerCount = static_cast<std::uint8_t>(game.players.size())};
}

}  // namespace

ServersService::ServersService(MasterDatabase& database)
    : servers_(database), samples_(database), changes_(database) {}

std::optional<network::CreatedServer> ServersService::createServer(
    const CreationAttempt& attempt) {
  std::string key = newServerKey();
  const std::optional<std::int64_t> serverId = servers_.insertWithin(
      NewServer{.name = attempt.creation.name,
                .keyFingerprint = fingerprintOf(key),
                .createdByAddress = attempt.address,
                .createdAt = attempt.requestedAt},
      CreationQuota{.maximum = MAXIMUM_CREATIONS_PER_ADDRESS,
                    .since = attempt.requestedAt - CREATION_WINDOW});
  if (!serverId.has_value()) {
    return std::nullopt;
  }
  return network::CreatedServer{
      .id = *serverId, .name = attempt.creation.name, .key = std::move(key)};
}

std::optional<KnownServer> ServersService::serverOfKey(std::string_view key) {
  if (!network::isWellFormedServerKey(key)) {
    return std::nullopt;
  }
  return servers_.serverOfKey(fingerprintOf(key));
}

void ServersService::replaceOwnServer(const ServerRecord& record) {
  servers_.saveRegistration(record);
}

bool ServersService::replaceOwnHeartbeat(const KnownServer& server,
                                         network::ServerHeartbeat heartbeat,
                                         std::chrono::sys_seconds now) {
  if (!server.limits.has_value()) {
    return false;
  }
  requireWithinLimits(heartbeat, *server.limits);
  const HeartbeatRecord record{.serverId = server.id,
                               .heartbeat = std::move(heartbeat),
                               .receivedAt = now};
  servers_.saveLastHeartbeat(record);
  samples_.add(record);
  return true;
}

std::optional<network::GameListing> ServersService::gameListingOf(
    std::int64_t serverId) {
  const std::optional<LastHeartbeat> last = servers_.lastHeartbeatOf(serverId);
  if (!last.has_value()) {
    return std::nullopt;
  }
  network::GameListing listing;
  if (last->heartbeat.has_value()) {
    for (const network::HostedGame& game : last->heartbeat->games) {
      listing.games.push_back(listedGameOf(game));
    }
  }
  return listing;
}

network::ServerListing ServersService::serverListingOf(
    const PageRequest& page) {
  return network::ServerListing{.servers = servers_.registeredServers(page)};
}

std::optional<network::StatusChangeListing>
ServersService::statusChangeListingOf(std::int64_t serverId,
                                      const PageRequest& page) {
  if (!servers_.hasServer(serverId)) {
    return std::nullopt;
  }
  return network::StatusChangeListing{.statusChanges =
                                          changes_.changesOf(serverId, page)};
}

}  // namespace rtype::master
