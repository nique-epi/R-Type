#include "MasterMessages.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "HostedGame.hpp"
#include "InformationConstants.hpp"
#include "ProtocolConstants.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerIdentity.hpp"
#include "ServerMeasures.hpp"
#include "ServerRegistration.hpp"

namespace rtype::server {

network::ServerRegistration registrationOf(const ServerIdentity& identity) {
  return network::ServerRegistration{
      .name = identity.name,
      .address = std::nullopt,
      .udpPort = identity.udpPort,
      .gameVersion = std::string{SERVER_GAME_VERSION},
      .protocolVersion = network::PROTOCOL_VERSION,
      .maximumGames = identity.maximumGames,
      .maximumPlayersPerGame =
          static_cast<std::uint8_t>(network::MAXIMUM_PLAYERS)};
}

network::ServerHeartbeat heartbeatOf(
    std::vector<network::HostedGame> games,
    const std::optional<network::ServerMeasures>& measures) {
  return network::ServerHeartbeat{
      .games = std::move(games),
      .measures = measures.value_or(network::ServerMeasures{})};
}

}  // namespace rtype::server
