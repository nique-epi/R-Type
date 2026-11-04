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
#include "HeartbeatRecord.hpp"
#include "HostedGame.hpp"
#include "MasterDatabase.hpp"
#include "MasterMessageConstants.hpp"
#include "NetworkException.hpp"
#include "RegisteredServer.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerLimits.hpp"
#include "ServerRecord.hpp"

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
    : repository_(database) {}

RegisteredServer ServersService::registerServer(const ServerRecord& record) {
  return repository_.save(record);
}

bool ServersService::recordHeartbeat(std::int64_t keyId,
                                     network::ServerHeartbeat heartbeat,
                                     std::chrono::sys_seconds now) {
  const std::optional<ServerLimits> limits = repository_.limitsOfKey(keyId);
  if (!limits.has_value()) {
    return false;
  }
  requireWithinLimits(heartbeat, *limits);
  repository_.recordHeartbeat(HeartbeatRecord{.serverId = limits->id,
                                              .heartbeat = std::move(heartbeat),
                                              .receivedAt = now});
  return true;
}

}  // namespace rtype::master
