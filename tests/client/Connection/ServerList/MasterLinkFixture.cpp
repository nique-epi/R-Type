#include "MasterLinkFixture.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <thread>
#include "CreatedServer.hpp"
#include "MasterAccess.hpp"
#include "MasterClient.hpp"
#include "MasterLink.hpp"
#include "MasterLinkTestConstants.hpp"
#include "ProtocolConstants.hpp"
#include "RunningMaster.hpp"
#include "ServerCreation.hpp"
#include "ServerListReading.hpp"
#include "ServerRegistration.hpp"

namespace rtype::client::testing {

MasterLinkFixture::MasterLinkFixture()
    : link_(network::MasterAccess{.url = master_.url(), .key = std::string{}}) {
  link_.start();
}

void MasterLinkFixture::TearDown() { link_.stop(); }

network::CreatedServer MasterLinkFixture::registeredServer() const {
  network::MasterClient anyone{
      network::MasterAccess{.url = master_.url(), .key = std::string{}}};
  const network::CreatedServer created = anyone.createServer(
      network::ServerCreation{.name = std::string{LINKED_SERVER_NAME}});
  network::MasterClient server{
      network::MasterAccess{.url = master_.url(), .key = created.key}};
  server.registerServer(network::ServerRegistration{
      .name = std::string{LINKED_SERVER_NAME},
      .address = std::string{LINKED_SERVER_ADDRESS},
      .udpPort = LINKED_SERVER_PORT,
      .gameVersion = std::string{LINKED_SERVER_GAME_VERSION},
      .protocolVersion = network::PROTOCOL_VERSION,
      .maximumGames = LINKED_SERVER_MAXIMUM_GAMES,
      .maximumPlayersPerGame = LINKED_SERVER_PLAYERS_PER_GAME});
  return created;
}

std::optional<ServerListReading> MasterLinkFixture::serversSoon(
    std::uint64_t ask) const {
  const auto deadline = std::chrono::steady_clock::now() + READING_DEADLINE;
  while (std::chrono::steady_clock::now() < deadline) {
    const std::optional<ServerListReading> reading = link_.servers();
    if (reading.has_value() && reading->ask == ask) {
      return reading;
    }
    std::this_thread::sleep_for(POLL_STEP);
  }
  return std::nullopt;
}

RunningMaster& MasterLinkFixture::master() { return master_; }

MasterLink& MasterLinkFixture::link() { return link_; }

}  // namespace rtype::client::testing
