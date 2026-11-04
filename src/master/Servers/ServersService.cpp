#include "ServersService.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
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
  return std::string{network::POINTER_SEPARATOR} +
         std::string{network::GAMES_MEMBER} +
         std::string{network::POINTER_SEPARATOR} + std::to_string(index) +
         std::string{network::POINTER_SEPARATOR} + std::string{member};
}

/**
 * @throws network::InvalidMasterMessageException naming the first member of
 * @p heartbeat beyond @p limits.
 */
void requireWithinLimits(const network::ServerHeartbeat& heartbeat,
                         const ServerLimits& limits) {
  if (heartbeat.games.size() > limits.maximumGames) {
    throw network::InvalidMasterMessageException(
        std::string{network::POINTER_SEPARATOR} +
        std::string{network::GAMES_MEMBER});
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
                                     const network::ServerHeartbeat& heartbeat,
                                     std::chrono::sys_seconds now) {
  const std::optional<ServerLimits> limits = repository_.limitsOfKey(keyId);
  if (!limits.has_value()) {
    return false;
  }
  requireWithinLimits(heartbeat, *limits);
  repository_.recordHeartbeat(HeartbeatRecord{
      .serverId = limits->id, .heartbeat = heartbeat, .receivedAt = now});
  return true;
}

}  // namespace rtype::master
