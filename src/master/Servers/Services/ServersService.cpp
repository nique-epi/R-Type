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
#include "HeartbeatRecord.hpp"
#include "HostedGame.hpp"
#include "KnownServer.hpp"
#include "MasterDatabase.hpp"
#include "MasterMessageConstants.hpp"
#include "MasterMessageText.hpp"
#include "NetworkException.hpp"
#include "NewServer.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerKey.hpp"
#include "ServerLimits.hpp"
#include "ServerRecord.hpp"
#include "ServersServiceConstants.hpp"

namespace rtype::master {

namespace {

std::string pointerOfGame(std::size_t index, std::string_view member) {
  return (nlohmann::json::json_pointer{} / std::string{network::GAMES_MEMBER} /
          index / std::string{member})
      .to_string();
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
  }
}

}  // namespace

ServersService::ServersService(MasterDatabase& database)
    : servers_(database), samples_(database) {}

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

}  // namespace rtype::master
