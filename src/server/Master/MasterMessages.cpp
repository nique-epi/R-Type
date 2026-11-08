#include "MasterMessages.hpp"
#include <cstdint>
#include <optional>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include "FinishedGame.hpp"
#include "FinishedGamePlayer.hpp"
#include "GameResult.hpp"
#include "HostedGame.hpp"
#include "InformationConstants.hpp"
#include "MasterMessageConstants.hpp"
#include "ProtocolConstants.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerIdentity.hpp"
#include "ServerMeasures.hpp"
#include "ServerRegistration.hpp"

namespace rtype::server {

std::uint64_t randomProbeSecret() {
  std::random_device device;
  std::uniform_int_distribution<std::uint64_t> secrets{
      network::LOWEST_PROBE_SECRET, network::HIGHEST_PROBE_SECRET};
  return secrets(device);
}

network::ServerRegistration registrationOf(const ServerIdentity& identity) {
  return network::ServerRegistration{
      .name = identity.name,
      .address = identity.address,
      .udpPort = identity.udpPort,
      .gameVersion = std::string{SERVER_GAME_VERSION},
      .protocolVersion = network::PROTOCOL_VERSION,
      .maximumGames = identity.maximumGames,
      .maximumPlayersPerGame =
          static_cast<std::uint8_t>(network::MAXIMUM_PLAYERS),
      .probeSecret = identity.probeSecret};
}

network::ServerHeartbeat heartbeatOf(
    std::vector<network::HostedGame> games,
    const std::optional<network::ServerMeasures>& measures,
    std::optional<std::uint64_t> probeToken) {
  return network::ServerHeartbeat{
      .games = std::move(games),
      .difficulties = {},
      .measures = measures.value_or(network::ServerMeasures{}),
      .probeToken = probeToken};
}

network::FinishedGame finishedGameOf(const GameResult& result) {
  std::vector<network::FinishedGamePlayer> players;
  players.reserve(result.players.size());
  for (const PlayerResult& player : result.players) {
    players.push_back(network::FinishedGamePlayer{.nickname = player.nickname,
                                                  .accountId = std::nullopt,
                                                  .score = player.score});
  }
  return network::FinishedGame{.playedAt = result.endedAt,
                               .difficulty = result.difficulty,
                               .playerCount = result.playerCount,
                               .vanilla = result.vanilla,
                               .players = std::move(players)};
}

}  // namespace rtype::server
